#include "mainWindow.h"
#include "ui_mainWindow.h"

#include "loginView.h"

//constructor
MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    stackedWidget(new QStackedWidget(this)),
    dataManager(DataManagement::getDataManager())
{
    ui->setupUi(this);

    //intialize managers & set arrays
    userManager = UserManagement::getUserManager();
    bookManager = BookManagement::getBookManager();
    transactionManager = TransactionManagement::getTransactionManager();

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
    connect(adminPage, &AdminView::logoutRequest, this, &::MainWindow::logOut);
    connect(memberPage, &MemberView::logoutRequest, this, &MainWindow::logOut);

    stackedWidget->setCurrentIndex(0);

    //read and store data
    dataManager->readData();
    userManager->setUserArray();
    bookManager->setBookArray();

    qDebug()<<"Main window created";
}

//deconstructor
MainWindow::~MainWindow(){
    delete ui;
}

//functions to change view display
void MainWindow::showLogin(){
    qDebug()<<"MainWindow: Displaying Login Screen";
    loginPage->clearInputs();
    loginPage->clearError();
    stackedWidget->setCurrentIndex(0);
}

void MainWindow::userLogin(const QString& username, QString password){

    //refreashing data
    bookManager->readData();
    bookManager->setBookArray();
    userManager->setUserArray();

    if(userManager->verifyLogin(username,password)){
        //set current user
        userManager->setCurrentUser(userManager->getUserObj(username));

        //clearing error messgaes
        loginPage->clearError();

        //checking userType
        if(userManager->isAdmin(username)){
            qDebug()<<"MainWindow: Admin Login Succesful. Loading Admin View";
            //populate admin views
            adminPage->displayUsers();
            adminPage->loadAdminCatalogue();
            //directing user to appropriate page
            stackedWidget->setCurrentIndex(2);
        } else {
            //else if member check that user in active
            if(userManager->isActive(username)){
                qDebug()<<"MainWindow: Member Login Succesful. Loading Member View";
                //populate member views
                memberPage->displayCurrentMember(userManager->getCurrentUser());
                memberPage->displayCheckedOut(userManager->getCurrentUser());
                memberPage->displayHoldRequests(userManager->getCurrentUser());
                memberPage->loadCatalogue();
                //directing user to appropriate page
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

void MainWindow::logOut() {
    //clearing current user
    userManager->clearCurrentUser();
    //reverting back to login page
    stackedWidget->setCurrentIndex(0);
    //clear previous inputs
    loginPage->clearInputs();
    qDebug()<<"User Logged out succesfully";
}


