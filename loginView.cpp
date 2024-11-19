#include "loginView.h"
#include "ui_loginView.h"
#include "mainWindow.h"


LoginView::LoginView(QWidget *parent) : QWidget(parent){
    ui.setupUi(this);

    //connect buttons
    connect(ui.loginButton, &QPushButton::clicked, this, &LoginView::loginButtonClicked);
    connect(ui.registerButton, &QPushButton::clicked, this, &LoginView::registerButtonClicked);

    //connecting signals from mainwidow
    MainWindow* mainWindow = dynamic_cast<MainWindow*>(parent);
    if(mainWindow){
        connect(mainWindow, &MainWindow::loginFail, this, &LoginView::loginError);
        connect(mainWindow, &MainWindow::notActiveAccount, this, &LoginView::notActiveError);
        qDebug()<<"Connection made for loginFail Signal";
    }

    ui.errorLabel->clear();
}

//view specific functions

void LoginView::loginButtonClicked() {
    QString username = ui.usernameLineEdit->text();
    QString password = ui.passwordLineEdit->text();
    emit loginAttempt(username, password);
    qDebug()<<"loginAttempt signal sent";
}

void LoginView::registerButtonClicked() {
    emit callRegisterView();
    qDebug()<<"callRegisterView signal sent";
}

void LoginView::loginError() {
    qDebug()<<"setting login error message";
    ui.errorLabel->setText("Invalid Login Details. Please try again");
}

void LoginView::notActiveError() {
    qDebug()<<"setting account error message";
    ui.errorLabel->setText("Accout is not Active. Please visit Library to Activate");
}

void LoginView::clearError(){
    ui.errorLabel->clear();
}
