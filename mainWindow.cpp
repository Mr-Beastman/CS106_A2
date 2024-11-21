#include "mainWindow.h"
#include "ui_mainWindow.h"

#include "loginView.h"
#include "registrationView.h"
#include "adminView.h"
#include "memberView.h"

//constructor
MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    stackedWidget(new QStackedWidget(this)),
    dataManager(new DataManagement()),
    userManager(new UserManagement()),
    bookManager(new bookManagement())
{
    ui->setupUi(this);

    // Set the stacked widget as the central widget of the main window
    setCentralWidget(stackedWidget);

    //creating views to be added into stackedWidget
    LoginView *loginPage = new LoginView(this);
    RegistrationView *registrationPage = new RegistrationView(this);
    AdminView *adminPage = new AdminView(this);

    //adding views to stackedWidget
    stackedWidget->addWidget(loginPage);
    stackedWidget->addWidget(registrationPage);
    stackedWidget->addWidget(adminPage);


    //connecting view signals

    connect(loginPage, &LoginView::loginAttempt, this, &MainWindow::userLogin);
    connect(loginPage, &LoginView::callRegisterView, this, &MainWindow::showRegister);
    connect(registrationPage, &RegistrationView::loginRequest, this, &MainWindow::showLogin);

    dataManager->readData();
    stackedWidget->setCurrentIndex(0);



    qDebug()<<"Main window created";
}

//deconstructor
MainWindow::~MainWindow(){
    delete ui;
}

DataManagement *MainWindow::getDataManager() {
    return dataManager;
}

UserManagement *MainWindow::getUserManager() {
    return userManager;
}

bookManagement *MainWindow::getBookManager() {
    return bookManager;
}

//functions to change view display
void MainWindow::showLogin(){
    qDebug()<<"Displaying Login Screen";
    stackedWidget->setCurrentIndex(0);
}

void MainWindow::userLogin(const QString& username, QString password){
    if(userManager->verifyLogin(username,password)){

        //clearing error messgaes
        LoginView *loginPage = dynamic_cast<LoginView*>(stackedWidget->widget(0));
        loginPage->clearError();

        //checking userType
        if(userManager->isAdmin(username)){
            qDebug()<<"Loading Admin View";
            userManager->setCurrentUser(username);
            stackedWidget->setCurrentIndex(2);
        } else {
            if(userManager->isActive(username)){
                qDebug()<<"Loading Member View";
                //creating memberPage with current user
                qDebug()<<("Setting current member as "+username);
                userManager->setCurrentUser(username);
                MemberView *memberPage = new MemberView(this);
                stackedWidget->addWidget(memberPage);
                stackedWidget->setCurrentIndex(3);
            } else {
                qDebug()<<"Member is not active";
                emit notActiveAccount();
            }

        }
    } else {
        qDebug()<<"loginFail signal sent";
        emit loginFail();
    }
}

void MainWindow::showRegister(){
    qDebug()<<"Displaying Registration Page";

    //clearing error messgaes
    LoginView *loginPage = dynamic_cast<LoginView*>(stackedWidget->widget(0));
    if(loginPage){
        loginPage->clearError();
    }

    stackedWidget->setCurrentIndex(1);
}

void MainWindow::logOut()
{
    //clearing current user
    userManager->setCurrentUser("");

    //deleting currentUser memberpage
    QWidget* memberPage = stackedWidget->widget(3);
    if(memberPage){
        stackedWidget->removeWidget(memberPage);
        delete memberPage;
    }

    stackedWidget->setCurrentIndex(0);
    qDebug()<<"User Logged out succesfully";
}


