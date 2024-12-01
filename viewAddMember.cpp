#include "viewAddMember.h"
#include "managementUser.h"
#include "qjsonobject.h"
#include "ui_viewAddMember.h"

ViewAddMember::ViewAddMember(QWidget *parent) : QDialog(parent), ui(new Ui::ViewAddMember) {
    ui->setupUi(this);

    // set up title
    this->setWindowTitle("Add Member");
    this->setModal(true);

    connect(ui->saveButton, &QPushButton::clicked, this, &ViewAddMember::addMemberButtonClicked);
}

ViewAddMember::~ViewAddMember() {
    delete ui;
}

void ViewAddMember::addMemberButtonClicked(){
    QJsonObject newUser;
    managementUser userManager;

    newUser["name"]=ui->nameLineEdit->text();
    newUser["username"]=ui->usernameLineEdit->text();
    newUser["password"]=ui->passwordLineEdit->text();
    newUser["phone"]=ui->phoneLineEdit->text();
    newUser["email"]=ui->emailLineEdit->text();
    newUser["address"]=ui->nameLineEdit->text();

    //checking if active box has been checked
    bool makeActive = ui->activeCheckBox->isChecked();
    if(makeActive){
        newUser["isActive"] = true;
    }

    //checking if admin box has been checked
    bool makeAdmin = ui->adminCheckBox->isChecked();
    if(makeAdmin){
        newUser["isAdmin"] = true;
    }

    if(!userManager.usernameExists(newUser)){
        userManager.addUser(newUser);
        accept();
    } else {
        qDebug()<<"viewAddMember: Setting username error message";
        ui->errorOutputLabel->setText("Username already taken. Please try again");
    }
}
