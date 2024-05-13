#include "loginpage.h"
#include "ui_loginpage.h"

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
        ui_->portLineEdit->text().toUShort()
    );
}
