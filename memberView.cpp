#include "memberView.h"
#include "qstackedwidget.h"
#include "updateUserView.h"
#include "bookManagement.h"
#include "userManagement.h"
#include "bookListView.h"

MemberView::MemberView(QWidget *parent) : QWidget(parent), ui(new Ui::MemberView) {
    ui->setupUi(this);

    //connect buttons
    connect(ui->updateButton, &QPushButton::clicked, this, &MemberView::updateButtonClicked);
    connect(ui->logoutButton, &QAbstractButton::clicked, this, &MemberView::logoutButtonClicked);

    //connect signals
    connect(ui->issuedList, &QListWidget::itemClicked, this, &MemberView::onBookClicked);
    connect(ui->holdList, &QListWidget::itemClicked, this, &MemberView::onBookClicked);
    connect(ui->catalogueList, &QListWidget::itemClicked, this, &MemberView::onBookClicked);
}

MemberView::~MemberView() {
    delete ui;
}

void MemberView::setAccountNumber(const QString &account){
    accountNumber = account;
}



QString &MemberView::getAccountNumber(){
    return accountNumber;
}

//view related functions

void MemberView::refreashMemberView(){
    qDebug()<<"MemberView: Refreashing Displays";
    displayCurrentMember();
    displayCheckedOut();
    displayHoldRequests();
    loadCatalogue();
}

void MemberView::displayCurrentMember() {
    UserManagement* userManager  = UserManagement::getUserManager();
    QJsonObject& currentUser = userManager->getCurrentUser();

    if(!currentUser.isEmpty()){
        qDebug()<<"MemberView: Populating Member Details";
        ui->nameOutputLabel->setText(currentUser["name"].toString());
        ui->accountOutputLabel->setText(currentUser["account"].toString());
        ui->phoneOutputLabel->setText(currentUser["phone"].toString());
        ui->emailOutputLabel->setText(currentUser["email"].toString());
        ui->addressOutputLabel->setText(currentUser["address"].toString());
    } else {
        qDebug()<<"MemberView: User Not found, cannot display information";
    }
}

//accountTab functions
void MemberView::updateButtonClicked() {
    qDebug()<<"MemberView: Setting up edit window";
    UpdateUserView* updateUserView = new UpdateUserView(this);
    updateUserView->setAccountNumber(ui->accountOutputLabel->text());
    updateUserView->preloadUser(ui->accountOutputLabel->text());

    qDebug()<<"MemberView: Loading edit window";
    updateUserView->exec();

    UserManagement* userManager = UserManagement::getUserManager();
    userManager->updateCurrentUser();
    displayCurrentMember();
}

void MemberView::logoutButtonClicked() {
    emit logoutRequest();
}

void MemberView::clearDisplay(){
    ui->issuedList->clear();
    ui->holdList->clear();
}

//populates ui with books currently checked out to the current user
//parameters: QjsonObject containing user to display
//returns : none
void MemberView::displayCheckedOut() {
    //getting mangers

    UserManagement* userManager = UserManagement::getUserManager();
    BookManagement* bookManager = BookManagement::getBookManager();

    //userManager->setUserArray();
    //getting user details
    QJsonObject& currentUser = userManager->getCurrentUser();
    qDebug()<<currentUser;
    QJsonArray checkedOut = currentUser["activeLoans"].toArray();

    qDebug()<<"MemberView: checked out array "<<checkedOut;
    qDebug()<<"-------------------Test------------------";
    QJsonObject user = userManager->getUserObjAccount(accountNumber);
    qDebug()<<user;
    QJsonArray test = user["activeLoans"].toArray();
    qDebug()<<test;
    qDebug()<<"-----------------------------------------";

    //if array is empty setting to no display
    if(checkedOut.isEmpty()){
        qDebug()<<"MemberView: Setting Checkedout display to empty";
        stackedWidgetDisplay(ui->issuedStackedWidget, ui->noIssuedPage);
        return;
    }

    //setting Checkout view to display lsit
    qDebug()<<"MemberView: Setting Checkedout Display checkedout Items";
    stackedWidgetDisplay(ui->issuedStackedWidget, ui->issuedPage);

    //ensuring clean list before populating
    ui->issuedList->clear();

    //building list to display
    qDebug()<<"MemberView: Generating users checkedout items";
    for(int i = 0; i<checkedOut.size(); ++i){
        QJsonObject entry = checkedOut[i].toObject();
        QJsonObject book = bookManager->getBookDetails(entry["isbn"].toString());

        //creating item
        BookListView* bookListItem = bookManager->createBookList(book, entry);

        //connect refrash signal
        connect(bookListItem, &BookListView::refreashMemberView, this, &MemberView::refreashMemberView);

        //display due date and renew options
        QStackedWidget* stackedWidget = bookListItem->findChild<QStackedWidget*>("optionsStackedWidget");
        int index = stackedWidget->indexOf(bookListItem->findChild<QWidget*>("userCheckedoutPage"));
        stackedWidget->setCurrentIndex(index);

        //adding entry to list
        QListWidgetItem* item = new QListWidgetItem(ui->issuedList);
        item->setSizeHint(bookListItem->sizeHint());

        //assigning isbn to item
        item->setData(Qt::UserRole, book["isbn"].toString());

        //adding alternating colors
        if(i % 2 == 0){
            item->setBackground(QBrush(QColor(158,206,104)));
        } else {
            item->setBackground(QBrush(QColor(187,211,180)));
        }

        ui->issuedList->setItemWidget(item, bookListItem);
    }

    qDebug()<<"MemberView: Issued items for "<<currentUser["username"].toString()<<" displayed";
}

void MemberView::displayHoldRequests() {
    //get managers
    BookManagement* bookManager = BookManagement::getBookManager();
    UserManagement* userManager = UserManagement::getUserManager();

    //get current user details
    QJsonObject& currentUser = userManager->getCurrentUser();
    QJsonArray holdRequests = currentUser["holdRequests"].toArray();

    //setting to display "no holds" if array empty
    if(holdRequests.isEmpty()){
        qDebug()<<"MemberView: Setting Hold display to empty";
        stackedWidgetDisplay(ui->holdStackedWidget, ui->noHoldPage);
        return;
    }

    //setting view to display list if array !empty
    qDebug()<<"MemberView: Setting hold to display items";
    stackedWidgetDisplay(ui->holdStackedWidget, ui->holdPage);

    //clean list view
    ui->holdList->clear();

    //build items for list to display
    for(int i = 0; i<holdRequests.size(); ++i){
        QJsonObject entry = holdRequests[i].toObject();
        QJsonObject book = bookManager->getBookDetails(entry["isbn"].toString());

        BookListView* bookListView = bookManager->createBookList(book, entry);

        //adding entry to list
        QListWidgetItem* item = new QListWidgetItem(ui->holdList);

        //adding alternating colors
        if(i % 2 == 0){
            item->setBackground(QBrush(QColor(158,206,104)));
        } else {
            item->setBackground(QBrush(QColor(187,211,180)));
        }

        connect(bookListView, &BookListView::refreashMemberView, this, &MemberView::refreashMemberView);

        item->setSizeHint(bookListView->sizeHint());

        //assigning isbn with item
        item->setData(Qt::UserRole, book["isbn"].toString());

        ui->holdList->setItemWidget(item, bookListView);
    }

    qDebug()<<"MemberView: Issued items for "<<currentUser["username"].toString()<<" displayed";
}

void MemberView::stackedWidgetDisplay(QStackedWidget *toSet, QWidget *page) {
    QStackedWidget* stackedWidget = toSet;
    int index = stackedWidget->indexOf(page);
    stackedWidget->setCurrentIndex(index);
}


//catalogueTab functions
void MemberView::loadCatalogue(){
    //getting managers
    BookManagement *bookManager = BookManagement::getBookManager();
    UserManagement *userManager = UserManagement::getUserManager();

    //getting userDetails
    QJsonArray bookData = bookManager->getBookArray();
    QJsonObject& currentUser = userManager->getCurrentUser();
    QJsonArray checkedOut = currentUser["activeLoans"].toArray();

    //clear list
    ui->catalogueList->clear();

    //looping throigh books to create a entry for it
    for(int i = 0; i<bookData.size(); ++i){
        QJsonObject book = bookData[i].toObject();

        bool userCheckedout = false;

        //checking if checked out to current user
        for(int j = 0; j<checkedOut.size(); j++){
            QJsonObject loaned = checkedOut[j].toObject();
            if(loaned["isbn"].toString() == book["isbn"].toString()){
                userCheckedout = true;
                break;
            }
        }

        QJsonObject entry;

        BookListView* bookListView = bookManager->createBookList(book, entry);

        //connect refrash signal
        connect(bookListView, &BookListView::refreashMemberView, this, &MemberView::refreashMemberView);

        //setting availability and member options
        if(!userCheckedout){
            if(bookManager->isAvailable(book["isbn"].toString())){
                QLabel* availabilityTextLabel = bookListView->findChild<QLabel*>("availabilityTextLabel");
                availabilityTextLabel->setText("Available");
                QStackedWidget* stackedWidget = bookListView->findChild<QStackedWidget*>("optionsStackedWidget");
                int index = stackedWidget->indexOf(bookListView->findChild<QWidget*>("userAvailablePage"));
                stackedWidget->setCurrentIndex(index);
            } else {
                QLabel* availabilityTextLabel = bookListView->findChild<QLabel*>("availabilityTextLabel");
                availabilityTextLabel->setText("Not Available");
                QStackedWidget* stackedWidget = bookListView->findChild<QStackedWidget*>("optionsStackedWidget");
                int index = stackedWidget->indexOf(bookListView->findChild<QWidget*>("userNotAvailablePage"));
                stackedWidget->setCurrentIndex(index);
            }
        } else {
            QStackedWidget* stackedWidget = bookListView->findChild<QStackedWidget*>("optionsStackedWidget");
            int index = stackedWidget->indexOf(bookListView->findChild<QWidget*>("userCheckedoutPage"));
            stackedWidget->setCurrentIndex(index);
        }

        //adding entry to list
        QListWidgetItem* item = new QListWidgetItem(ui->catalogueList);

        //adding alternating colors
        if(i % 2 == 0){
            item->setBackground(QBrush(QColor(158,206,104)));
        } else {
            item->setBackground(QBrush(QColor(187,211,180)));
        }

        item->setSizeHint(bookListView->sizeHint());

        //assigning isbn with item
        item->setData(Qt::UserRole, book["isbn"].toString());

        ui->catalogueList->setItemWidget(item, bookListView);
    }
}

void MemberView::onBookClicked(QListWidgetItem *book) {
    //getting the assigned isbn from clicked item
    QString isbn = book->data(Qt::UserRole).toString();

    //getting book details
    BookManagement* bookManger = BookManagement::getBookManager();
    QJsonObject bookDetails = bookManger->getBookDetails(isbn);

    qDebug()<<"MemberView: Generating Book Info View";

    emit requestBookInfo(bookDetails);
}
