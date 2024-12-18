#include "viewMemberDashboard.h"
#include "qstackedwidget.h"
#include "managementTransaction.h"
#include "ui_viewMemberDashboard.h"
#include "viewUpdateUser.h"
#include "ManagementBook.h"
#include "viewBookItem.h"

ViewMemberDashboard::ViewMemberDashboard(QWidget *parent) : QWidget(parent), ui(new Ui::ViewMemberDashboard) {
    ui->setupUi(this);

    //connect buttons
    connect(ui->updateButton, &QPushButton::clicked, this, &ViewMemberDashboard::updateButtonClicked);
    connect(ui->logoutButton, &QPushButton::clicked, this, &ViewMemberDashboard::logoutButtonClicked);

    //connect signals
    connect(ui->issuedList, &QListWidget::itemClicked, this, &ViewMemberDashboard::onBookClicked);
    connect(ui->holdList, &QListWidget::itemClicked, this, &ViewMemberDashboard::onBookClicked);
    connect(ui->catalogueList, &QListWidget::itemClicked, this, &ViewMemberDashboard::onBookClicked);
}

ViewMemberDashboard::~ViewMemberDashboard() {
    delete ui;
}

void ViewMemberDashboard::setAccountNumber(const QString &account){
    accountNumber = account;
}

void ViewMemberDashboard::setUsername(const QString &account){
    username = account;
}

QString &ViewMemberDashboard::getAccountNumber(){
    return accountNumber;
}

//view related functions

void ViewMemberDashboard::updateDisplays(){
    qDebug()<<"viewMemberDashboard: Refreashing Displays";
    emit refreashMemberDisplay();
}

void ViewMemberDashboard::displayCurrentMember() {
    ManagementUser userManager;
    QJsonObject currentUser = userManager.getUserObj(username);

    if(!currentUser.isEmpty()){
        qDebug()<<"viewMemberDashboard: Populating Member Details";
        ui->nameOutputLabel->setText(currentUser["name"].toString());
        ui->accountOutputLabel->setText(currentUser["account"].toString());
        ui->phoneOutputLabel->setText(currentUser["phone"].toString());
        ui->emailOutputLabel->setText(currentUser["email"].toString());
        ui->addressOutputLabel->setText(currentUser["address"].toString());
    } else {
        qDebug()<<"viewMemberDashboard: User Not found, cannot display information";
    }
}

//accountTab functions
void ViewMemberDashboard::updateButtonClicked() {
    ViewUpdateUser* viewUpdateUser = new ViewUpdateUser(this);
    viewUpdateUser->setAccountNumber(ui->accountOutputLabel->text());
    viewUpdateUser->preloadUser(ui->accountOutputLabel->text());
    viewUpdateUser->exec();
    displayCurrentMember();
}

void ViewMemberDashboard::logoutButtonClicked() {
    emit logoutRequest();
}

void ViewMemberDashboard::clearDisplay(){
    ui->issuedList->clear();
    ui->holdList->clear();
}

//populates ui with books currently checked out to the current user
//parameters: none
//returns : none
void ViewMemberDashboard::displayCheckedOut() {
    //getting mangers

    ManagementUser userManager;
    ManagementBook bookManager;
    ManagementTransaction transactionManager;

    QJsonObject currentUser = userManager.getUserObj(username);
    QJsonArray activeLoans = currentUser["activeLoans"].toArray();

    qDebug()<<"ViewMemberDashboard: Checking for users active loans";

    //if array is empty setting to no display
    if(activeLoans.isEmpty()){
        qDebug()<<"ViewMemberDashboard: No active loans found for user";
        stackedWidgetDisplay(ui->issuedStackedWidget, ui->noIssuedPage);
        ui->issuedList->hide();
        return;
    }

    //setting Checkout view to display lsit
    qDebug()<<"ViewMemberDashboard: Active loans found for user";
    stackedWidgetDisplay(ui->issuedStackedWidget, ui->issuedPage);

    //ensuring clean list before populating
    ui->issuedList->show();
    ui->issuedList->clear();

    //building list to display
    qDebug()<<"ViewMemberDashboard: Generating users loan items";
    for(int i = 0; i<activeLoans.size(); ++i){
        QJsonObject entry = activeLoans[i].toObject();
        QJsonObject book = bookManager.getBookDetails(entry["isbn"].toString());

        //creating item
        ViewBookItem* bookListItem = bookManager.createBookList(book, entry);

        //connect refreash signal doing in book list view
        connect(bookListItem, &ViewBookItem::refreashviewMemberDashboard, this, &ViewMemberDashboard::updateDisplays);

        //set book availbity and the options aavialble
        transactionManager.setBookAvailibityOptions(bookListItem, book, username);

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
}

void ViewMemberDashboard::displayHoldRequests() {
    //get managers
    ManagementBook bookManager;
    ManagementUser userManager;
    ManagementTransaction transactionManager;

    //get current user details
    QJsonArray holdsArray = transactionManager.getHoldArray();
    QJsonArray userHolds;

    qDebug()<<"ViewMemberDashboard: Checking for users active holds";
    for(int i = 0; i<holdsArray.size(); i++){
        QJsonObject holdEntry = holdsArray[i].toObject();
        if(holdEntry["username"].toString() == username){
            userHolds.append(holdEntry);
        }
    }

    //setting to display "no holds" if array empty
    if(userHolds.isEmpty()){
        qDebug()<<"ViewMemberDashboard: No active holds found for user";
        stackedWidgetDisplay(ui->holdStackedWidget, ui->noHoldPage);
        ui->holdList->hide();
        return;
    }

    //setting view to display list if array !empty
    qDebug()<<"ViewMemberDashboard: Active holds found for user";
    stackedWidgetDisplay(ui->holdStackedWidget, ui->holdPage);

    //clean list view
    ui->holdList->clear();
    ui->holdList->show();

    qDebug()<<"ViewMemberDashboard: Generating users hold items";
    //build items for list to display
    for(int i = 0; i<userHolds.size(); ++i){
        QJsonObject hold = userHolds[i].toObject();
        QJsonObject book = bookManager.getBookDetails(hold["isbn"].toString());

        //creating viewBookItem for hold
        ViewBookItem* viewBookItem = bookManager.createBookList(book, hold);

        //connect refreash signal doing in book list view
        connect(viewBookItem, &ViewBookItem::refreashviewMemberDashboard, this, &ViewMemberDashboard::updateDisplays);

        //adding entry to list
        QListWidgetItem* item = new QListWidgetItem(ui->holdList);

        //adding alternating colors
        if(i % 2 == 0){
            item->setBackground(QBrush(QColor(158,206,104)));
        } else {
            item->setBackground(QBrush(QColor(187,211,180)));
        }

        connect(viewBookItem, &ViewBookItem::refreashviewMemberDashboard, this, &ViewMemberDashboard::updateDisplays);

        item->setSizeHint(viewBookItem->sizeHint());

        //storing hidden values for transactions
        QString holdId = hold["holdId"].toString();
        QLabel* idLabel = viewBookItem->findChild<QLabel*>("holdStoredIdLabel");
        idLabel->setText(holdId);
        QLabel* usernameLabel = viewBookItem->findChild<QLabel*>("usernameStoredLabel");
        usernameLabel->setText(username);

        //setting availability and members action displays
        //set book availbity and the options aavialble
        transactionManager.setBookAvailibityOptions(viewBookItem, book, username);
        ui->holdList->setItemWidget(item, viewBookItem);
    }
}

void ViewMemberDashboard::stackedWidgetDisplay(QStackedWidget *toSet, QWidget *page) {
    QStackedWidget* stackedWidget = toSet;
    int index = stackedWidget->indexOf(page);
    stackedWidget->setCurrentIndex(index);
}


//catalogueTab functions
void ViewMemberDashboard::displayCatalogue() {
    //get managers
    ManagementBook bookManager;
    ManagementUser userManager;
    ManagementTransaction transactionManager;

    //get user and book data
    QJsonObject& database = userManager.getFileData();
    QJsonArray bookData = database["books"].toArray();
    QJsonObject currentUser = userManager.getUserObj(username);
    QJsonArray holdsArray = transactionManager.getHoldArray();

    //clear the catalogue list
    ui->catalogueList->clear();

    //loop through and set up each book
    for (int i = 0; i < bookData.size(); ++i) {
        QJsonObject book = bookData[i].toObject();
        QString isbn = book["isbn"].toString();
        QJsonArray bookQueue = book["inQueue"].toArray();

        QJsonObject entry;
        ViewBookItem* viewBookItem = bookManager.createBookList(book, entry);

        //skip the book if view creation fails
        if (!viewBookItem) {
            qDebug() << "Failed to create viewBookItem for ISBN:" << isbn;
            continue;
        }

        QListWidgetItem* item = new QListWidgetItem(ui->catalogueList);

        //add alternating colors for list items
        if(i % 2 == 0){
            item->setBackground(QBrush(QColor(158,206,104)));
        } else {
            item->setBackground(QBrush(QColor(187,211,180)));
        }
        item->setSizeHint(viewBookItem->sizeHint());
        item->setData(Qt::UserRole, isbn);

        //store username and hold count
        QLabel* userLabel = viewBookItem->findChild<QLabel*>("usernameStoredLabel");
        if (userLabel) userLabel->setText(username);

        QLabel* holdArrayLabel = viewBookItem->findChild<QLabel*>("holdTextLabel");
        if (holdArrayLabel) holdArrayLabel->setText(QString::number(bookQueue.size()));

        //set book availbity and the options aavialble
        transactionManager.setBookAvailibityOptions(viewBookItem, book, username);

        ui->catalogueList->setItemWidget(item, viewBookItem);

        //connect signals for refreshing the view
        connect(viewBookItem, &ViewBookItem::refreashviewMemberDashboard, this, &ViewMemberDashboard::updateDisplays);
    }

    qDebug() << "Catalogue loaded successfully.";
}


void ViewMemberDashboard::onBookClicked(QListWidgetItem *book) {

    QString isbn = book->data(Qt::UserRole).toString();

    //getting book details
    ManagementBook bookManager;
    QJsonObject bookDetails = bookManager.getBookDetails(isbn);

    qDebug()<<"viewMemberDashboard: Generating Book Info View";

    emit requestBookInfo(bookDetails,username);
}
