#include "p2p_messenger.h"
#include "./ui_p2p_messenger.h"

#include <iostream>
#include <string>
#include <thread>
#include <utility>

// need this to avoid conflict with Qt's emit keyword
#ifdef emit
#undef emit
#include <syncstream>
#define emit
#endif

P2PMessenger::P2PMessenger(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::P2PMessenger)
    , dht_ip_resolver{dht_port}
    , num_threads{4}
    , io_context{static_cast<int>(num_threads)}
    , work_guard{net::make_work_guard(io_context)}
    , message_receiver{io_context, my_port}
    , message_sender{io_context, dht_ip_resolver, my_login}
{
    ui->setupUi(this);

    // put data on the dht
    dht_ip_resolver.put(my_login, my_ip, my_port);

    // listen for data on the dht
    dht_ip_resolver.listen(destination_login);
    dht_ip_resolver.listen(my_login);

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

    message_receiver.start();

    //udp::socket send_socket{io_context, udp::v4()};
}

P2PMessenger::~P2PMessenger()
{
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "P2PMessenger destructor called" << std::endl;
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Shutting down gracefully..." << std::endl;
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Stopping work_guard" << std::endl;
    work_guard.reset();
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Stopped work_guard" << std::endl;
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Stopping io_context" << std::endl;
    io_context.stop();
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Stopped io_context" << std::endl;
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Joining threads" << std::endl;
    for (std::size_t i = 0; i < num_threads; ++i) {
        threads[i].join();
    }
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Joined threads" << std::endl;
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Deleting ui" << std::endl;
    delete ui;
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Deleted ui" << std::endl;
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "P2PMessenger destructor finished" << std::endl;
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
            std::optional<std::string> destination_address = dht_ip_resolver.Resolve(destination_login);
            if (destination_address) {
                std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Destination address is set" << std::endl;
                std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Sending message to " << destination_login << '\n';
                std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Calling send_message" << '\n';
                net::post(io_context, [this, message]() {
                    message_sender.send_message(destination_login, message);
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
