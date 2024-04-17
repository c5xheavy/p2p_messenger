#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <iostream>
#include <string>
#include <thread>

// need this to avoid conflict with Qt's emit keyword
#ifdef emit
#undef emit
#include <syncstream>
#define emit
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_sendButton_clicked()
{
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Send button is clicked" << std::endl;
    std::string message = ui->messageEdit->text().toStdString();
    if (message.empty()) {
        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Message is empty" << std::endl;
        return;
    }
    ui->messageEdit->clear();
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Sending message: " << message << std::endl;
}
