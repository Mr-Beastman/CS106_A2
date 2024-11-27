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
    }

    ui.errorLabel->clear();

    qDebug()<<"LoginView: Login in created displayed";
}

//view specific functions

void LoginView::loginButtonClicked() {
    QString username = (ui.usernameLineEdit->text()).toLower();
    QString password = ui.passwordLineEdit->text();
    emit loginAttempt(username, password);
}

void LoginView::registerButtonClicked() {
    emit callRegisterView();
}

void LoginView::loginError() {
    ui.errorLabel->setText("Invalid Login Details. Please try again");
}

void LoginView::notActiveError() {
    ui.errorLabel->setText("Accout is not Active. Please visit Library to Activate");
}

void LoginView::clearError(){
    ui.errorLabel->clear();
}

void LoginView::clearInputs() {
    ui.usernameLineEdit->clear();
    ui.passwordLineEdit->clear();
}
