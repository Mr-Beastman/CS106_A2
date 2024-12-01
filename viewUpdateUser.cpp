#include "viewUpdateUser.h"
#include "ui_viewUpdateUser.h"
#include "mainWindow.h"

ViewUpdateUser::ViewUpdateUser(QWidget *parent) : QDialog(parent), ui(new Ui::ViewUpdateUser) {
    ui->setupUi(this);

    // set up title
    this->setWindowTitle("Update Details");
    this->setModal(true);

    connect(ui->saveButton, &QPushButton::clicked, this, &ViewUpdateUser::saveButtonClicked);
}

ViewUpdateUser::~ViewUpdateUser() {
    delete ui;
}

void ViewUpdateUser::setAccountNumber(const QString& account){
    accountNumber = account;
}

QString ViewUpdateUser::getAccountNumber(){
    return accountNumber;
}

void ViewUpdateUser::preloadUser(const QString& account) {

    managementUser userManager;

    QJsonObject user = userManager.getUserObj(account);

    //preload lineEdits
    ui->nameLineEdit->setText(user["name"].toString());
    ui->phoneLineEdit->setText(user["phone"].toString());
    ui->emailLineEdit->setText(user["email"].toString());
    ui->addressLineEdit->setText(user["address"].toString());

}

void ViewUpdateUser::saveButtonClicked() {

    managementUser userManager;

    QJsonObject updateDetails;

    updateDetails["name"] = ui->nameLineEdit->text();
    updateDetails["phone"] = ui->phoneLineEdit->text();
    updateDetails["email"] = ui->emailLineEdit->text();
    updateDetails["address"] = ui->addressLineEdit->text();


    if(userManager.updateUser(accountNumber,updateDetails)){
        accept();
    } else {
        qDebug()<<"viewUpdateUser: Displaying Error Message";
        ui->errorLabel->setText("No updates detected");
    }
}
