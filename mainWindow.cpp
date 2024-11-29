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

    //intialize original managers & set arrays
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
    bookInfoPage = new BookInfoView(this);

    //adding views to stackedWidget
    stackedWidget->addWidget(loginPage);
    stackedWidget->addWidget(registrationPage);
    stackedWidget->addWidget(adminPage);
    stackedWidget->addWidget(memberPage);
    stackedWidget->addWidget(bookInfoPage);


    //connecting view signals
    connect(loginPage, &LoginView::loginAttempt, this, &MainWindow::userLogin);
    connect(loginPage, &LoginView::callRegisterView, this, &MainWindow::showRegister);
    connect(registrationPage, &RegistrationView::loginRequest, this, &MainWindow::showLogin);
    connect(adminPage, &AdminView::logoutRequest, this, &::MainWindow::logOut);
    connect(adminPage, &AdminView::requestBookInfo, this, &MainWindow::showBookInfo);
    connect(memberPage, &MemberView::logoutRequest, this, &MainWindow::logOut);
    connect(memberPage, &MemberView::requestBookInfo, this, &MainWindow::showBookInfo);
    connect(bookInfoPage, &BookInfoView::goBack, this, &MainWindow::goBack);

    stackedWidget->setCurrentIndex(0);

    loadData();

    qDebug()<<"MainWindow: Main window created";
}

//deconstructor
MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::loadData(){
    dataManager->readData();
    userManager->setUserArray();
    bookManager->setBookArray();
}

//functions to change view display
void MainWindow::showLogin(){
    qDebug()<<"MainWindow: Displaying Login Screen";

    //ensure clean view and set display to login page
    loginPage->clearInputs();
    loginPage->clearError();
    stackedWidget->setCurrentIndex(0);
}

void MainWindow::userLogin(const QString& username, QString password){

    loadData();

    if(userManager->verifyLogin(username,password)){
        //set current user
        userManager->setCurrentUser(userManager->getUserObj(username));

        //clearing error messgaes
        loginPage->clearError();

        //checking userType
        if(userManager->isAdmin(username)){
            qDebug()<<"MainWindow: Admin Login Succesful. Loading Admin View";

            //populate admin dashboards
            adminPage->displayUsers();
            adminPage->loadAdminCatalogue();

            //directing to admin page
            stackedWidget->setCurrentIndex(2);
        } else if(userManager->isActive(username)) {
            qDebug()<<"MainWindow: Member Login Succesful. Loading Member View";

            //populate member views
            memberPage->displayCurrentMember();
            memberPage->displayCheckedOut();
            memberPage->displayHoldRequests();
            memberPage->loadCatalogue();

            //directing to user page
            stackedWidget->setCurrentIndex(3);
        } else {
                qDebug()<<"MainWindow: Member is not active";
                emit notActiveAccount();
        }
    } else {
        qDebug()<<"MainWindow: loginFail signal sent";
        emit loginFail();
    }
}

void MainWindow::showRegister(){
    qDebug()<<"MainWindow: Displaying Registration Page";
    stackedWidget->setCurrentIndex(1);
}

void MainWindow::showBookInfo(QJsonObject &bookDetails) {
    bookInfoPage->setBookDetails(bookDetails);
    bookInfoPage->setOptions();
    stackedWidget->setCurrentIndex(4);
}

void MainWindow::goBack(){
    QJsonObject& currentUser = userManager->getCurrentUser();

    if(userManager->isAdmin(currentUser["username"].toString())){
        stackedWidget->setCurrentIndex(2);
    } else {
        stackedWidget->setCurrentIndex(3);
    }
}

void MainWindow::logOut() {
    //clearing current user and switch back to login page
    userManager->clearCurrentUser();
    stackedWidget->setCurrentIndex(0);

    //reload data for next user
    loadData();

    //clear previous inputs
    loginPage->clearInputs();
    qDebug()<<"MainWindow: User Logged out succesfully";
}
