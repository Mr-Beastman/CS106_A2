#include "mainWindow.h"
#include "ui_mainWindow.h"

#include "loginView.h"

//constructor
MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    stackedWidget(new QStackedWidget(this)),
    dataManager(new DataManagement()),
    userManager(UserManagement::getUserManager()),
    bookManager(BookManagement::getBookManager())
{
    ui->setupUi(this);

    if(dataManager->readData()){
        qDebug()<<"MainWindow: Library Database Loaded";
    } else {
        qFatal()<<"MainWindow: Library Failed to Load, user cannot log in. Closing Program";
    }

    // Set the stacked widget as the central widget of the main window
    setCentralWidget(stackedWidget);

    //creating views to be added into stackedWidget
    loginPage = new LoginView(this);
    registrationPage = new RegistrationView(this);
    adminPage = new AdminView(this);
    memberPage = new MemberView(this);

    //adding views to stackedWidget
    stackedWidget->addWidget(loginPage);
    stackedWidget->addWidget(registrationPage);
    stackedWidget->addWidget(adminPage);
    stackedWidget->addWidget(memberPage);


    //connecting view signals

    connect(loginPage, &LoginView::loginAttempt, this, &MainWindow::userLogin);
    connect(loginPage, &LoginView::callRegisterView, this, &MainWindow::showRegister);
    connect(registrationPage, &RegistrationView::loginRequest, this, &MainWindow::showLogin);

    stackedWidget->setCurrentIndex(0);



    qDebug()<<"Main window created";
}

//deconstructor
MainWindow::~MainWindow(){
    delete ui;
}

//functions to change view display
void MainWindow::showLogin(){
    qDebug()<<"MainWindow: Displaying Login Screen";
    stackedWidget->setCurrentIndex(0);
}

void MainWindow::userLogin(const QString& username, QString password){
    if(userManager->verifyLogin(username,password)){

        //clearing error messgaes
        LoginView *loginPage = dynamic_cast<LoginView*>(stackedWidget->widget(0));
        loginPage->clearError();

        //checking userType
        if(userManager->isAdmin(username)){
            qDebug()<<"MainWindow: Admin Login Succesful. Loading Admin View";

            //recording current user
            userManager->setCurrentUser(username);

            //populate admin views
            adminPage->displayUsers();

            //direct user to page
            stackedWidget->setCurrentIndex(2);
        } else {
            if(userManager->isActive(username)){
                qDebug()<<"MainWindow: Member Login Succesful. Loading Member View";

                //recording current user
                userManager->setCurrentUser(username);

                memberPage->displayCurrentMember(userManager->getCurrentUser());

                stackedWidget->addWidget(memberPage);
                stackedWidget->setCurrentIndex(3);
            } else {
                qDebug()<<"Member is not active";
                emit notActiveAccount();
            }

        }
    } else {
        qDebug()<<"MainWindow: loginFail signal sent";
        emit loginFail();
    }
}

void MainWindow::showRegister(){
    qDebug()<<"MainWindow: Displaying Registration Page";

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


