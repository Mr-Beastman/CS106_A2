#include "viewRegistration.h"
#include "ui_viewRegistration.h"

ViewRegistration::ViewRegistration(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ViewRegistration)
{
    ui->setupUi(this);

    connect(ui->cancelButton, &QPushButton::clicked, this, &ViewRegistration::cancelButtonClicked);
    connect(ui->submitButton, &QPushButton::clicked, this, &ViewRegistration::submitButtonClicked);
}

ViewRegistration::~ViewRegistration() {
    delete ui;
}


void ViewRegistration::submitButtonClicked() {

    managementUser userManager;

    QJsonObject newUser;

    newUser["name"] = ui->nameLineEdit->text();
    newUser["username"] = (ui->usernameLineEdit->text()).toLower();
    newUser["password"] = ui->passwordLineEdit->text();
    newUser["phone"] = ui->phoneLineEdit->text();
    newUser["email"] = ui->emailLineEdit->text();
    newUser["address"] = ui->addressLineEdit->text();

    //direct calling as signals could not connect
    if(!userManager.usernameExists(newUser)){
        bool memberAdded = userManager.addUser(newUser);

        if(memberAdded){
            qDebug()<<"viewRegistration: Member added Succesfully";
            //return to login screen
            emit loginRequest();
        } else {
            qDebug()<<"viewRegistration: Failed to add member";
        }
    } else {
        qDebug()<<"viewRegistration: Username already exists";
        ui->errorLabel->setText("Username already taken. Please try again");
    }
}

void ViewRegistration::cancelButtonClicked() {
    emit loginRequest();
}
