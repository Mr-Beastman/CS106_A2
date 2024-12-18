#include "mainWindow.h"
#include "managementTransaction.h"
#include "ui_mainWindow.h"

#include "viewLogin.h"

//constructor
MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    stackedWidget(new QStackedWidget(this))
{
    ui->setupUi(this);

    this->setWindowTitle("Library Information System");

    // Set the stacked widget as the central widget of the main window
    setCentralWidget(stackedWidget);

    //creating views to be added into stackedWidget
    loginPage = new ViewLogin(this);
    registrationPage = new ViewRegistration(this);
    adminPage = new ViewAdminDashboard(this);
    memberPage = new ViewMemberDashboard(this);
    bookInfoPage = new ViewBookInfo(this);
    memberInfoPage = new ViewMemberInfo(this);

    //adding views to stackedWidget
    stackedWidget->addWidget(loginPage);
    stackedWidget->addWidget(registrationPage);
    stackedWidget->addWidget(adminPage);
    stackedWidget->addWidget(memberPage);
    stackedWidget->addWidget(bookInfoPage);
    stackedWidget->addWidget(memberInfoPage);


    //connecting view signals
    connect(loginPage, &ViewLogin::loginAttempt, this, &MainWindow::userLogin);
    connect(loginPage, &ViewLogin::callRegisterView, this, &MainWindow::showRegister);
    connect(registrationPage, &ViewRegistration::loginRequest, this, &MainWindow::showLogin);
    connect(adminPage, &ViewAdminDashboard::logoutRequest, this, &::MainWindow::logOut);
    connect(adminPage, &ViewAdminDashboard::requestBookInfo, this, &MainWindow::showBookInfo);
    connect(adminPage, &ViewAdminDashboard::requestMemberInfo, this, &MainWindow::showMemberInfo);
    connect(memberPage, &ViewMemberDashboard::logoutRequest, this, &MainWindow::logOut);
    connect(memberPage, &ViewMemberDashboard::requestBookInfo, this, &MainWindow::showBookInfo);
    connect(memberPage, &ViewMemberDashboard::refreashMemberDisplay, this, &MainWindow::updateMemberDisplays);
    connect(bookInfoPage, &ViewBookInfo::goBack, this, &MainWindow::goBack);
    connect(bookInfoPage, &ViewBookInfo::logoutRequest, this, &MainWindow::logOut);
    connect(bookInfoPage, &ViewBookInfo::refreashMemberDisplay, this, &MainWindow::updateMemberDisplays);
    connect(bookInfoPage, &ViewBookInfo::refreashAdminDisplay, this, &MainWindow::updateAdminDisplays);
    connect(memberInfoPage, &ViewMemberInfo::logoutRequest, this, &MainWindow::logOut);
    connect(memberInfoPage, &ViewMemberInfo::goBackAdmin, this, &MainWindow::goBackAdmin);
    connect(memberInfoPage, &ViewMemberInfo::requestUpdateDisplay, this, &MainWindow::updateAdminDisplays);


    stackedWidget->setCurrentIndex(0);

    loadData();


    qDebug()<<"MainWindow: Main window created";
}

//deconstructor
MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::loadData(){
    ManagementData dataManager;
    dataManager.readData();
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
    ManagementUser userManager;
    ManagementTransaction transactionManager;

    //using sign in to force loan status updates as it is regulary called
    transactionManager.updateOverdueLoans();
    transactionManager.updateDueLoans();

    if(userManager.verifyLogin(username,password)){
        //set current user
        userManager.setCurrentUser(username);
        loggedIn = username;

        //clearing error messgaes
        loginPage->clearError();

        //checking userType
        if(userManager.isAdmin(username)){
            qDebug()<<"MainWindow: Admin Login Succesful. Loading Admin View";

            //populate admin dashboards
            adminPage->setAdminUser(username);
            adminPage->displayUsers();
            adminPage->displayNewUsers();
            adminPage->displayOverdueBooks();
            adminPage->displayAdminCatalogue();

            //directing to admin page
            stackedWidget->setCurrentIndex(2);
        } else if(userManager.isActive(username)) {
            qDebug()<<"MainWindow: Member Login Succesful. Loading Member View";

            //setting variables used in memberpage
            memberPage->setAccountNumber(userManager.getAccount(username));
            memberPage->setUsername(username);

            //populate member views
            qDebug()<<"MainWindow: Populating Member Dashboard";
            memberPage->displayCurrentMember();
            memberPage->displayCheckedOut();
            memberPage->displayHoldRequests();
            memberPage->displayCatalogue();
            qDebug()<<"MainWindow: Member setup and redirecting";

            //directing to user page
            stackedWidget->setCurrentIndex(3);
        } else {
            //signal to emit error to user that account not active
                qDebug()<<"MainWindow: Member is not active";
                emit notActiveAccount();
        }
    } else {
        //signal to emit error to user that details are invalid
        qDebug()<<"MainWindow: loginFail signal sent";
        emit loginFail();
    }
}

void MainWindow::showRegister(){
    //ensure latest data
    loadData();
    //change views
    qDebug()<<"MainWindow: Displaying Registration Page";
    stackedWidget->setCurrentIndex(1);
}

void MainWindow::showBookInfo(QJsonObject &bookDetails, const QString& username) {
    bookInfoPage->setBookDetails(bookDetails);
    bookInfoPage->setCurrentUser(username);
    bookInfoPage->setBookAvailibity(bookDetails);
    bookInfoPage->populateCurrentHolds(bookDetails);

    ManagementUser userManager;
    if(userManager.isAdmin(loggedIn)){
        bookInfoPage->showAdminInfo();
        qDebug()<<"MainWindow: Displaying admin info for book view";
    } else {
        bookInfoPage->hideAdminInfo();
        qDebug()<<"MainWindow: Hiding admin info for book view";
    }
    stackedWidget->setCurrentIndex(4);
}

void MainWindow::showMemberInfo(QJsonObject &userToView, const QString& adminUser){
    memberInfoPage->setMemberDetails(userToView);
    memberInfoPage->setAdminUser(adminUser);
    memberInfoPage->generateCheckedout(userToView["account"].toString());
    memberInfoPage->displayHoldRequests(userToView["account"].toString());
    stackedWidget->setCurrentIndex(5);
}

void MainWindow::updateAdminDisplays(){
    qDebug()<<"MainWindow: Refreashing Member Displays";
    adminPage->updateDisplays();
}

void MainWindow::updateMemberDisplays(){
    memberPage->displayCheckedOut();
    memberPage->displayHoldRequests();
    memberPage->displayCatalogue();
}

void MainWindow::goBack(){
    qDebug()<<"MainWindow: Going back to memberview";
    ManagementUser userManager;
    if(userManager.isAdmin(loggedIn)){
        stackedWidget->setCurrentIndex(2);
    } else {
    stackedWidget->setCurrentIndex(3);
    }
}

void MainWindow::goBackAdmin(){
    stackedWidget->setCurrentIndex(2);
}

void MainWindow::logOut() {
    //clearing current user and switch back to login page
    ManagementUser userManager;
    ManagementData dataManager;
    userManager.clearCurrentUser();
    stackedWidget->setCurrentIndex(0);

    //making sure to clear all stored data
    dataManager.clearData();

    //reload for next user
    loadData();

    //clear previous inputs
    loginPage->clearInputs();
    loginPage->clearError();

    memberPage->clearDisplay();
    qDebug()<<"MainWindow: User Logged out succesfully";

}
