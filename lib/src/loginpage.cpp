#include "loginpage.h"
#include "ui_loginpage.h"

LoginPage::LoginPage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginPage)
{
    ui->setupUi(this);
}

LoginPage::~LoginPage()
{
    delete ui;
}

void LoginPage::on_loginPushButton_clicked()
{
    emit on_Login_LoginPage(
        ui->loginLineEdit->text().toStdString(),
        ui->DHTPortLineEdit->text().toUShort(),
        ui->IPLineEdit->text().toStdString(),
        ui->portLineEdit->text().toUShort()
    );
}
