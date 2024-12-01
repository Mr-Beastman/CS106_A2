#include "viewLogin.h"
#include "ui_viewLogin.h"
#include "mainWindow.h"


ViewLogin::ViewLogin(QWidget *parent) : QWidget(parent){
    ui.setupUi(this);

    //connect buttons
    connect(ui.loginButton, &QPushButton::clicked, this, &ViewLogin::loginButtonClicked);
    connect(ui.registerButton, &QPushButton::clicked, this, &ViewLogin::registerButtonClicked);

    //connecting signals from mainwidow
    MainWindow* mainWindow = dynamic_cast<MainWindow*>(parent);
    if(mainWindow){
        connect(mainWindow, &MainWindow::loginFail, this, &ViewLogin::loginError);
        connect(mainWindow, &MainWindow::notActiveAccount, this, &ViewLogin::notActiveError);
    }

    ui.errorLabel->clear();

    qDebug()<<"viewLogin: Login in created displayed";
}

//view specific functions

void ViewLogin::loginButtonClicked() {
    QString username = (ui.usernameLineEdit->text()).toLower();
    QString password = ui.passwordLineEdit->text();
    emit loginAttempt(username, password);
}

void ViewLogin::registerButtonClicked() {
    emit callRegisterView();
}

void ViewLogin::loginError() {
    ui.errorLabel->setText("Invalid Login Details. Please try again");
}

void ViewLogin::notActiveError() {
    ui.errorLabel->setText("Accout is not Active. Please visit Library to Activate");
}

void ViewLogin::clearError(){
    ui.errorLabel->clear();
}

void ViewLogin::clearInputs() {
    ui.usernameLineEdit->clear();
    ui.passwordLineEdit->clear();
}
