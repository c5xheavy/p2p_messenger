#include "p2p_messenger.h"
#include "./ui_p2p_messenger.h"

#include <iostream>
#include <string>
#include <thread>

// need this to avoid conflict with Qt's emit keyword
#ifdef emit
#undef emit
#include <syncstream>
#define emit
#endif

P2PMessenger::P2PMessenger(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::P2PMessenger)
    , node{}
    , num_threads{4}
    , io_context{num_threads}
    , work_guard{net::make_work_guard(io_context)}
    , receive_socket{io_context, udp::endpoint{udp::v4(), my_port}}
    , send_socket{io_context, udp::v4()}
{
    ui->setupUi(this);

    // Launch a dht node on a new thread, using a
    // generated RSA key pair, and listen on port.
    node.run(dht_port, dht::crypto::generateIdentity(), true);

    // Join the network through any running node,
    // here using a known bootstrap node.
    node.bootstrap("bootstrap.jami.net", "4222");

    // put data on the dht
    std::string my_address{my_ip + ":" + std::to_string(my_port)};
    node.put(dht::InfoHash::get(my_login), {(const std::uint8_t*)my_address.data(), my_address.size()});

    // Заполняем список логинов
    {
        std::lock_guard<std::mutex> lock{logins_mutex};
        logins.push_back(destination_login);
    }

    {
        std::lock_guard<std::mutex> lock{logins_mutex};
        for (const auto& login : logins) {
            auto key = dht::InfoHash::get(login);
            std::future<size_t> token = node.listen(key,
                [this, login](const std::vector<std::shared_ptr<dht::Value>>& values, bool expired) {
                    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "listen callback called for login " << login << std::endl;
                    for (const auto& value : values) {
                        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Found value: " << *value << ", " << (expired ? "expired" : "added") << std::endl;
                        std::string address;
                        for (const char& x : value->data) {
                            address += x;
                        }
                        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Found address: " << address << std::endl;
                        if (!expired) {
                            std::size_t pos = address.find(':');
                            if (pos == std::string::npos) {
                                throw std::invalid_argument{"Invalid address"};
                            }
                            {
                                std::lock_guard<std::mutex> lock{login_to_address_mutex};
                                login_to_address[login] = address;
                            }
                            std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Update destination address to " << address << std::endl;
                        }
                    }
                    return true; // keep listening
                }
            );
            {
                std::lock_guard<std::mutex> lock{login_to_token_mutex};
                login_to_token[login] = std::move(token);
            }
        }
    }

    // const unsigned num_threads = std::thread::hardware_concurrency();

    //net::io_context io_context;

    // net::executor_work_guard<net::io_context::executor_type> work_guard = net::make_work_guard(io_context);

    // std::vector<std::thread> threads;
    threads.reserve(num_threads);
    for (std::size_t i = 0; i < num_threads; ++i) {
        threads.emplace_back([this] {
            std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Starting io_context" << std::endl;
            io_context.run();
            std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Stopped io_context" << std::endl;
        });
    }

    //udp::socket receive_socket{io_context, udp::endpoint{udp::v4(), my_port}};

    std::function<void(const sys::error_code&)> async_wait_handler = [this, async_wait_handler](const sys::error_code& ec) {
        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Got message" << std::endl;
        if (!ec) {
            std::size_t bytes_available{receive_socket.available()};
            std::shared_ptr<char[]> buffer{new char[bytes_available]};

            udp::endpoint remote_endpoint;
            std::size_t buffer_size{receive_socket.receive_from(net::buffer(buffer.get(), bytes_available), remote_endpoint)};

            if (bytes_available != buffer_size) {
                throw std::logic_error{"Bytes available is not equal bytes read"};
            }

            Message message{MessageDeserializer::MessageFromBuffer(buffer.get(), buffer_size)};

            std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Received message:" << std::endl;
            std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << message.id << std::endl;
            std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << message.source_login_hash << std::endl;
            std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << message.destination_login_hash << std::endl;
            std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << message.payload.time << std::endl;
            std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << message.payload.text << std::endl;
            std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "End of message" << std::endl;
        } else {
            std::osyncstream(std::cerr) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Wait message error: " << ec.what() << std::endl;
        }

        receive_socket.async_wait(udp::socket::wait_read, async_wait_handler);
    };

    receive_socket.async_wait(udp::socket::wait_read, async_wait_handler);

    //udp::socket send_socket{io_context, udp::v4()};
}

P2PMessenger::~P2PMessenger()
{
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "P2PMessenger destructor called" << std::endl;
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Shutting down gracefully..." << std::endl;
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Stopping work_guard" << std::endl;
    work_guard.reset();
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Stopped work_guard" << std::endl;
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Closing send_socket" << std::endl;
    send_socket.close();
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Closed send_socket" << std::endl;
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Closing receive_socket" << std::endl;
    receive_socket.close();
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Closed receive_socket" << std::endl;
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Stopping io_context" << std::endl;
    io_context.stop();
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Stopped io_context" << std::endl;
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Joining threads" << std::endl;
    for (std::size_t i = 0; i < num_threads; ++i) {
        threads[i].join();
    }
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Joined threads" << std::endl;
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Cancelling listen" << std::endl;
    {
        std::lock_guard<std::mutex> lock{login_to_token_mutex};
        for (auto& [login, token] : login_to_token) {
            auto key = dht::InfoHash::get(login);
            node.cancelListen(key, std::move(token));
        }
    }
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Cancelled listen" << std::endl;
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Joining node" << std::endl;
    node.join();
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Joined node" << std::endl;
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Deleting ui" << std::endl;
    delete ui;
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Deleted ui" << std::endl;
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "P2PMessenger destructor finished" << std::endl;
}

std::pair<std::string, std::uint16_t> P2PMessenger::get_ip_and_port_from_address(const std::string& address) {
    std::size_t pos = address.find(':');
    if (pos == std::string::npos) {
        throw std::invalid_argument{"Invalid address"};
    }
    std::string ip{address.substr(0, pos)};
    std::uint16_t port = std::stoi(address.substr(pos + 1));
    return {ip, port};
}

void P2PMessenger::send_message(udp::socket& socket, const std::string& destination_login, const std::string& text) {
    try {
        Message message {
            1,
            LoginHasher::Hash("source"),
            LoginHasher::Hash("destination"),
            {
                1700000000,
                text
            }
        };
        auto [buffer, buffer_size]{MessageSerializer::MessageToBuffer(message)};

        std::string address;
        {
            std::lock_guard<std::mutex> lock{login_to_address_mutex};
            if (!login_to_address.contains(destination_login)) {
                throw std::logic_error{"Destination address is not set"};
            }
            address = login_to_address[destination_login];
        }
        auto [destination_ip, destination_port]{get_ip_and_port_from_address(address)};

        udp::endpoint endpoint{net::ip::make_address(destination_ip), destination_port};
        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Send message to " << destination_ip << ':' << destination_port << std::endl;
        socket.send_to(net::buffer(buffer.get(), buffer_size), endpoint);
    } catch (std::exception& e) {
        std::cerr << "Exception in send_message: " << e.what() << std::endl;
    }
}

void P2PMessenger::on_send_message() {
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "on_send_message called" << std::endl;
    std::string message = ui->messageEdit->text().toStdString();
    if (message.empty()) {
        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Message is empty" << std::endl;
        return;
    }
    ui->messageEdit->clear();
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Sending message: " << message << std::endl;
    net::post(io_context, [this, message]() {{
            std::lock_guard<std::mutex> lock{login_to_address_mutex};
            if (login_to_address.contains(destination_login)) {
                std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Destination address is set" << std::endl;
                std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Sending message to " << destination_login << '\n';
                std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Calling send_message" << '\n';
                net::post(io_context, [this, message]() {
                    send_message(send_socket, destination_login, message);
                });
                std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Called send_message" << '\n';
            } else {
                std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Destination address is not set" << std::endl;
            }
        }});
}

void P2PMessenger::on_sendButton_clicked()
{
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Send button is clicked" << std::endl;
    on_send_message();
}

void P2PMessenger::on_messageEdit_returnPressed()
{
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Return key is pressed" << std::endl;
    on_send_message();
}
