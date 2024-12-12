#include "viewUpdateUser.h"
#include "managementUser.h"
#include "ui_viewUpdateUser.h"

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

    ManagementUser userManager;

    QJsonObject user = userManager.getUserObjAccount(account);

    //preload lineEdits
    ui->nameLineEdit->setText(user["name"].toString());
    ui->phoneLineEdit->setText(user["phone"].toString());
    ui->emailLineEdit->setText(user["email"].toString());
    ui->addressLineEdit->setText(user["address"].toString());

}

void ViewUpdateUser::saveButtonClicked() {

    ManagementUser userManager;

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
