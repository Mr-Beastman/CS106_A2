#include "viewMemberDashboard.h"
#include "qstackedwidget.h"
#include "managementTransaction.h"
#include "ui_viewMemberDashboard.h"
#include "viewUpdateUser.h"
#include "managementBook.h"
#include "managementUser.h"
#include "viewBookItem.h"

ViewMemberDashboard::ViewMemberDashboard(QWidget *parent) : QWidget(parent), ui(new Ui::ViewMemberDashboard) {
    ui->setupUi(this);

    //connect buttons
    connect(ui->updateButton, &QPushButton::clicked, this, &ViewMemberDashboard::updateButtonClicked);
    connect(ui->logoutButton, &QAbstractButton::clicked, this, &ViewMemberDashboard::logoutButtonClicked);

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
    managementUser userManager;
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
    qDebug()<<"viewMemberDashboard: Setting up edit window";
    ViewUpdateUser* viewUpdateUser = new ViewUpdateUser(this);
    viewUpdateUser->setAccountNumber(ui->accountOutputLabel->text());
    viewUpdateUser->preloadUser(username);

    qDebug()<<"viewMemberDashboard: Loading edit window";
    viewUpdateUser->exec();

    managementUser userManager;
    //userManager.updateCurrentUser();
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
//parameters: QjsonObject containing user to display
//returns : none
void ViewMemberDashboard::displayCheckedOut() {
    //getting mangers

    managementUser userManager;
    managementBook bookManager;

    //userManager.setUserArray();
    //getting user details
    //get data

    QJsonObject currentUser = userManager.getUserObj(username);
    QJsonArray activeLoans = currentUser["activeLoans"].toArray();

    //if array is empty setting to no display
    if(activeLoans.isEmpty()){
        qDebug()<<"viewMemberDashboard: Setting Checkedout display to empty";
        stackedWidgetDisplay(ui->issuedStackedWidget, ui->noIssuedPage);
        return;
    }

    //setting Checkout view to display lsit
    qDebug()<<"viewMemberDashboard: Setting Checkedout Display checkedout Items";
    stackedWidgetDisplay(ui->issuedStackedWidget, ui->issuedPage);

    //ensuring clean list before populating
    ui->issuedList->clear();

    //building list to display
    qDebug()<<"viewMemberDashboard: Generating users checkedout items";
    for(int i = 0; i<activeLoans.size(); ++i){
        QJsonObject entry = activeLoans[i].toObject();
        QJsonObject book = bookManager.getBookDetails(entry["isbn"].toString());

        //creating item
        ViewBookItem* bookListItem = bookManager.createBookList(book, entry);

        // //connect refreash signal doing in book list view
        connect(bookListItem, &ViewBookItem::refreashviewMemberDashboard, this, &ViewMemberDashboard::updateDisplays);

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

    qDebug()<<"viewMemberDashboard: Issued items for "<<currentUser["username"].toString()<<" displayed";
}

void ViewMemberDashboard::displayHoldRequests() {
    //get managers
    managementBook bookManager;
    managementUser userManager;

    //get current user details
    QJsonObject currentUser = userManager.getUserObj(username);
    QJsonArray holdRequests = currentUser["holdRequests"].toArray();

    //setting to display "no holds" if array empty
    if(holdRequests.isEmpty()){
        qDebug()<<"viewMemberDashboard: Setting Hold display to empty";
        stackedWidgetDisplay(ui->holdStackedWidget, ui->noHoldPage);
        return;
    }

    //setting view to display list if array !empty
    qDebug()<<"viewMemberDashboard: Setting hold to display items";
    stackedWidgetDisplay(ui->holdStackedWidget, ui->holdPage);

    //clean list view
    ui->holdList->clear();

    //build items for list to display
    for(int i = 0; i<holdRequests.size(); ++i){
        QJsonObject entry = holdRequests[i].toObject();
        QJsonObject book = bookManager.getBookDetails(entry["isbn"].toString());

        ViewBookItem* viewBookItem = bookManager.createBookList(book, entry);

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

        //assigning isbn with item
        item->setData(Qt::UserRole, book["holdId"].toString());

        QString holdId = entry["holdId"].toString();

        QLabel* idLabel = viewBookItem->findChild<QLabel*>("holdIdOutputLabel");
        idLabel->setText(holdId);

        qDebug()<<idLabel->text();

        managementTransaction transactionManager;

        if(transactionManager.checkHoldstatus(holdId) == "ready"){
            QStackedWidget* stackedWidget = viewBookItem->findChild<QStackedWidget*>("optionsStackedWidget");
            int index = stackedWidget->indexOf(viewBookItem->findChild<QWidget*>("userReadyPage"));
            stackedWidget->setCurrentIndex(index);
        } else {
            QStackedWidget* stackedWidget = viewBookItem->findChild<QStackedWidget*>("optionsStackedWidget");
            int index = stackedWidget->indexOf(viewBookItem->findChild<QWidget*>("holdActivePage"));
            stackedWidget->setCurrentIndex(index);
        }

        ui->holdList->setItemWidget(item, viewBookItem);
    }

    qDebug()<<"viewMemberDashboard: Issued items for "<<currentUser["username"].toString()<<" displayed";
}

void ViewMemberDashboard::stackedWidgetDisplay(QStackedWidget *toSet, QWidget *page) {
    QStackedWidget* stackedWidget = toSet;
    int index = stackedWidget->indexOf(page);
    stackedWidget->setCurrentIndex(index);
}


//catalogueTab functions
void ViewMemberDashboard::loadCatalogue() {
    //set managers
    managementBook bookManager;
    managementUser userManager;
    managementTransaction transactionManager;

    //get data
    QJsonObject& database = userManager.getFileData();

    QJsonArray bookData = database["books"].toArray();
    QJsonObject currentUser = userManager.getUserObj(username);
    QJsonArray activeLoans = currentUser["activeLoans"].toArray();

    //clear list to ensure clean build
    ui->catalogueList->clear();

    //setting up books
    for (int i = 0; i < bookData.size(); ++i) {
        QJsonObject book = bookData[i].toObject();

        //checking book is valid
        if (!book.contains("isbn") || !book["isbn"].isString()) {
            qDebug() << "Skipping invalid book entry at index" << i;
            continue;
        }

        QString isbn = book["isbn"].toString();

        QJsonObject entry;
        ViewBookItem* viewBookItem = bookManager.createBookList(book, entry);
        if (!viewBookItem) {
            qDebug() << "Failed to create viewBookItem for ISBN:" << isbn;
            continue;
        }

        QListWidgetItem* item = new QListWidgetItem(ui->catalogueList);

        //adding alternating colors
        if(i % 2 == 0){
            item->setBackground(QBrush(QColor(158,206,104)));
        } else {
            item->setBackground(QBrush(QColor(187,211,180)));
        }

        item->setSizeHint(viewBookItem->sizeHint());
        item->setData(Qt::UserRole, isbn);

        //store hidden data for transactions
        QLabel* userLabel = viewBookItem->findChild<QLabel*>("usernameStoredLabel");
        if(userLabel){
            userLabel->setText(username);
        }

        //adding info to display
        QLabel* activeLoanLabel = viewBookItem->findChild<QLabel*>("CheckedOutputLabel");
        if (activeLoanLabel) {
            activeLoanLabel->setText(transactionManager.checkedOutTo(isbn));
        }

        QLabel* holdArrayLabel = viewBookItem->findChild<QLabel*>("QueueOutputLabel");
        if (holdArrayLabel) {
            QJsonArray inQueue = book["inQueue"].toArray();
            holdArrayLabel->setText(QString::number(inQueue.size()));
        }

        //set availabilty display and user options
        QStackedWidget* availabilityWidget = viewBookItem->findChild<QStackedWidget*>("availabilityWidget");
        QStackedWidget* optionsWidget = viewBookItem->findChild<QStackedWidget*>("optionsStackedWidget");

        if (availabilityWidget && optionsWidget) {
            //checking if another user has the book
            bool isLoanedByUser = false;
            for (const auto& loan : activeLoans) {
                if (loan.toObject()["isbn"].toString() == isbn) {
                    isLoanedByUser = true;
                    break;
                }
            }

            if (book["isAvailable"].toBool()) {
                QWidget* availablePage = viewBookItem->findChild<QWidget*>("availablePage");
                QWidget* userAvailablePage = viewBookItem->findChild<QWidget*>("userAvailablePage");

                if (availablePage) {
                    int index = availabilityWidget->indexOf(availablePage);
                    availabilityWidget->setCurrentIndex(index);
                }
                if (userAvailablePage) {
                    int index = optionsWidget->indexOf(userAvailablePage);
                    optionsWidget->setCurrentIndex(index);
                }
            } else if (isLoanedByUser) {
                // current user has active loan for book
                QWidget* checkedOutPage = viewBookItem->findChild<QWidget*>("userCheckedoutPage");
                QWidget* checkedoutPage = viewBookItem->findChild<QWidget*>("checkedoutPage");

                if (checkedOutPage) {
                    int index = availabilityWidget->indexOf(checkedoutPage);
                    availabilityWidget->setCurrentIndex(index);

                    index = optionsWidget->indexOf(checkedOutPage);
                    optionsWidget->setCurrentIndex(index);

                    // Populate due date
                    QLabel* dueDateLabel = viewBookItem->findChild<QLabel*>("dueOutputLabel");
                    if (dueDateLabel) {
                        QString dueDateText = "N/A";
                        for (const auto& loan : activeLoans) {
                            QJsonObject loanObj = loan.toObject();
                            if (loanObj["isbn"].toString() == isbn && loanObj.contains("dueDate")) {
                                dueDateText = loanObj["dueDate"].toString();
                                break;
                            }
                        }
                        dueDateLabel->setText(dueDateText);
                    } else {
                        qDebug() << "Error: Due date label not found for ISBN:" << isbn;
                    }
                }
            } else {

                //not available and not checked out to current user
                QWidget* notAvailablePage = viewBookItem->findChild<QWidget*>("notAvailablePage");
                QWidget* userNotAvailablePage = viewBookItem->findChild<QWidget*>("userNotAvailablePage");

                if (notAvailablePage) {
                    int index = availabilityWidget->indexOf(notAvailablePage);
                    availabilityWidget->setCurrentIndex(index);
                }
                if (userNotAvailablePage) {
                    int index = optionsWidget->indexOf(userNotAvailablePage);
                    optionsWidget->setCurrentIndex(index);
                }
            }
        } else {
            qDebug() << "Error: Missing availabilityWidget or optionsWidget for ISBN:" << isbn;
        }

        ui->catalogueList->setItemWidget(item, viewBookItem);

        connect(viewBookItem, &ViewBookItem::refreashviewMemberDashboard, this, &ViewMemberDashboard::updateDisplays);
    }

    qDebug() << "Catalogue loaded successfully.";
}


void ViewMemberDashboard::onBookClicked(QListWidgetItem *book) {

    QString isbn = book->data(Qt::UserRole).toString();

    //getting book details
    managementBook bookManager;
    QJsonObject bookDetails = bookManager.getBookDetails(isbn);

    qDebug()<<"viewMemberDashboard: Generating Book Info View";

    emit requestBookInfo(bookDetails);
}
