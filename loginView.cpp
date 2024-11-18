#include "loginView.h"
#include "ui_loginView.h"
#include "mainWindow.h"


LoginView::LoginView(QWidget *parent) :
    QWidget(parent){
    ui.setupUi(this);

    //connect buttons
    connect(ui.loginButton, &QPushButton::clicked, this, &LoginView::loginButtonClicked);
    connect(ui.registerButton, &QPushButton::clicked, this, &LoginView::registerButtonClicked);

    MainWindow* mainWindow = dynamic_cast<MainWindow*>(parent);
    if(mainWindow){
        connect(mainWindow, &MainWindow::loginFail, this, &LoginView::onLoginFail);
        qDebug()<<"Connection made for loginFail Signal";
    }

    ui.errorLabel->clear();
}

//view specific functions

void LoginView::loginButtonClicked(){
    QString username = ui.usernameLineEdit->text();
    QString password = ui.passwordLineEdit->text();
    emit loginAttempt(username, password);
    qDebug()<<"loginAttempt signal sent";
}

void LoginView::registerButtonClicked(){
    emit callRegisterView();
    qDebug()<<"callRegisterView signal sent";
}

void LoginView::onLoginFail(){
    qDebug()<<"setting error message";
    ui.errorLabel->setText("Invalid Login Details. Please try again");
}

void LoginView::clearError(){
    ui.errorLabel->clear();
}
