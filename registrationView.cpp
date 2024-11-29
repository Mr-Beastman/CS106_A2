#include "registrationView.h"
#include "ui_registrationView.h"

RegistrationView::RegistrationView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RegistrationView)
{
    ui->setupUi(this);

    connect(ui->cancelButton, &QPushButton::clicked, this, &RegistrationView::cancelButtonClicked);
    connect(ui->submitButton, &QPushButton::clicked, this, &RegistrationView::submitButtonClicked);
}

RegistrationView::~RegistrationView() {
    delete ui;
}


void RegistrationView::submitButtonClicked() {

    UserManagement* userManager = UserManagement::getUserManager();

    QJsonObject newUser;

    newUser["name"] = ui->nameLineEdit->text();
    newUser["username"] = (ui->usernameLineEdit->text()).toLower();
    newUser["password"] = ui->passwordLineEdit->text();
    newUser["phone"] = ui->phoneLineEdit->text();
    newUser["email"] = ui->emailLineEdit->text();
    newUser["address"] = ui->addressLineEdit->text();

    //direct calling as signals could not connect
    if(!userManager->usernameExists(newUser)){
        bool memberAdded = userManager->addUser(newUser);

        if(memberAdded){
            qDebug()<<"RegistrationView: Member added Succesfully";
            //return to login screen
            emit loginRequest();
        } else {
            qDebug()<<"RegistrationView: Failed to add member";
        }
    } else {
        qDebug()<<"RegistrationView: Username already exists";
        ui->errorLabel->setText("Username already taken. Please try again");
    }
}

void RegistrationView::cancelButtonClicked() {
    emit loginRequest();
}
