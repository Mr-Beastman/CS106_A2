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

    QString nameInput = ui->nameLineEdit->text();
    QString usernameInput = (ui->usernameLineEdit->text()).toLower();
    QString passwordInput = ui->passwordLineEdit->text();
    QString phoneInput = ui->phoneLineEdit->text();
    QString emailInput = ui->emailLineEdit->text();
    QString addressInput = ui->addressLineEdit->text();

    //direct calling as signals could not connect
    bool memberAdded = userManager->addUser(nameInput,
                                            usernameInput,
                                            passwordInput,
                                            phoneInput,
                                            emailInput,
                                            addressInput);

    if(memberAdded){
        qDebug()<<"Member added Succesfully";
        //return to login screen
        emit loginRequest();
    } else {
        qDebug()<<"Failed to add member";
    }
}

void RegistrationView::cancelButtonClicked() {
    emit loginRequest();
}
