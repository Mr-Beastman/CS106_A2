#include "mainWindow.h"
#include "ui_mainWindow.h"

#include "loginView.h"
#include "registrationView.h"
#include "utilities.h"

//constructor
MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    stackedWidget(new QStackedWidget(this)),
    userManager(new UserManagement(Utilities::setDatabasePath()))
{
    ui->setupUi(this);


    // Set the stacked widget as the central widget of the main window
    setCentralWidget(stackedWidget);

    //creating views to be added into stackedWidget
    LoginView *loginPage = new LoginView(this);
    RegistrationView *registrationPage = new RegistrationView(this);

    //adding views to stackedWidget
    stackedWidget->addWidget(loginPage);
    stackedWidget->addWidget(registrationPage);

    //connecting view signals

    connect(loginPage, &LoginView::loginAttempt, this, &MainWindow::userLogin);
    connect(loginPage, &LoginView::callRegisterView, this, &MainWindow::showRegister);
    connect(registrationPage, &::RegistrationView::loginRequest, this, &MainWindow::showLogin);

    stackedWidget->setCurrentIndex(0);

    qDebug()<<"Main window created";
}

//deconstructor
MainWindow::~MainWindow()
{
    delete ui;
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
        if(loginPage){
            loginPage->clearError();
        }

        //checking userType
        if(userManager->isAdmin(username)){
            qDebug()<<"Loading Admin View";
        } else {
            qDebug()<<"Loading Member View";
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


