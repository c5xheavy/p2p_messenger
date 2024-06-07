#include "loginpage.h"
#include "ui_loginpage.h"

#include <iostream>
#include <stdexcept>

#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp> 

namespace net = boost::asio;
using net::ip::tcp;

LoginPage::LoginPage(QWidget *parent) :
    QDialog(parent),
    ui_(new Ui::LoginPage) {
    ui_->setupUi(this);
}

LoginPage::~LoginPage() {
    delete ui_;
}

void LoginPage::on_loginPushButton_clicked() {
    emit log_in_requsted(
        ui_->loginLineEdit->text().toStdString(),
        ui_->DHTPortLineEdit->text().toUShort(),
        ui_->IPLineEdit->text().toStdString(),
        ui_->portLineEdit->text().toUShort(),
        ui_->bootstrapNodeIPLineEdit->text().toStdString(),
        ui_->bootstrapNodePortLineEdit->text().toUShort(),
        ui_->relayNodeIPLineEdit->text().toStdString(),
        ui_->relayNodePortLineEdit->text().toUShort(),
        ui_->serveAsRelayCheckBox->isChecked(),
        ui_->generateCryptoIdentityCheckBox->isChecked(),
        ui_->cryptoIdentityPathLineEdit->text().toStdString()
    );
}

void LoginPage::on_resolvePushButton_clicked() {
    try {
        net::io_context io_context;
        tcp::resolver resolver{io_context};
        tcp::resolver::results_type endpoints = resolver.resolve("ipv4.icanhazip.com", "http");

        tcp::socket socket{io_context};
        net::connect(socket, endpoints);

        std::string request = "GET / HTTP/1.1\r\n";
        request += "Host: ipv4.icanhazip.com\r\n";
        request += "Connection: close\r\n\r\n";

        net::write(socket, net::buffer(request));

        net::streambuf response;
        net::read_until(socket, response, "\r\n\r\n");

        std::istream response_stream{&response};
        std::string http_version;
        unsigned int status_code;
        std::string status_message;

        response_stream >> http_version;
        response_stream >> status_code;
        std::getline(response_stream, status_message);

        std::string header;
        while (std::getline(response_stream, header) && header != "\r") {}

        if (status_code == 200) {
            std::ostringstream ss;
            ss << &response;
            std::string ip = ss.str();
            boost::algorithm::trim(ip);
            ui_->IPLineEdit->setText(QString::fromStdString(ip));
        } else {
            std::cout << "Failed to get IP address. HTTP status code: " << status_code << std::endl;
        }
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}
