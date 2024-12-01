#include "memberView.h"
#include "qstackedwidget.h"
#include "transactionManagement.h"
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

void MemberView::setUsername(const QString &account){
    username = account;
}



QString &MemberView::getAccountNumber(){
    return accountNumber;
}

//view related functions

void MemberView::updateDisplays(){
    qDebug()<<"MemberView: Refreashing Displays";
    emit refreashMemberDisplay();
}

void MemberView::displayCurrentMember() {
    UserManagement userManager;
    QJsonObject currentUser = userManager.getUserObj(username);

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
    updateUserView->preloadUser(username);

    qDebug()<<"MemberView: Loading edit window";
    updateUserView->exec();

    UserManagement userManager;
    //userManager.updateCurrentUser();
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

    UserManagement userManager;
    BookManagement bookManager;

    //userManager.setUserArray();
    //getting user details
    //get data

    QJsonObject currentUser = userManager.getUserObj(username);
    QJsonArray activeLoans = currentUser["activeLoans"].toArray();

    //if array is empty setting to no display
    if(activeLoans.isEmpty()){
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
    for(int i = 0; i<activeLoans.size(); ++i){
        QJsonObject entry = activeLoans[i].toObject();
        QJsonObject book = bookManager.getBookDetails(entry["isbn"].toString());

        //creating item
        BookListView* bookListItem = bookManager.createBookList(book, entry);

        // //connect refreash signal doing in book list view
        connect(bookListItem, &BookListView::refreashMemberView, this, &MemberView::updateDisplays);

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
    BookManagement bookManager;
    UserManagement userManager;

    //get current user details
    QJsonObject currentUser = userManager.getUserObj(username);
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
        QJsonObject book = bookManager.getBookDetails(entry["isbn"].toString());

        BookListView* bookListView = bookManager.createBookList(book, entry);

        //adding entry to list
        QListWidgetItem* item = new QListWidgetItem(ui->holdList);

        //adding alternating colors
        if(i % 2 == 0){
            item->setBackground(QBrush(QColor(158,206,104)));
        } else {
            item->setBackground(QBrush(QColor(187,211,180)));
        }

        connect(bookListView, &BookListView::refreashMemberView, this, &MemberView::updateDisplays);

        item->setSizeHint(bookListView->sizeHint());

        //assigning isbn with item
        item->setData(Qt::UserRole, book["holdId"].toString());

        QString holdId = entry["holdId"].toString();

        QLabel* idLabel = bookListView->findChild<QLabel*>("holdIdOutputLabel");
        idLabel->setText(holdId);

        qDebug()<<idLabel->text();

        TransactionManagement transactionManager;

        if(transactionManager.checkHoldstatus(holdId) == "ready"){
            QStackedWidget* stackedWidget = bookListView->findChild<QStackedWidget*>("optionsStackedWidget");
            int index = stackedWidget->indexOf(bookListView->findChild<QWidget*>("userReadyPage"));
            stackedWidget->setCurrentIndex(index);
        } else {
            QStackedWidget* stackedWidget = bookListView->findChild<QStackedWidget*>("optionsStackedWidget");
            int index = stackedWidget->indexOf(bookListView->findChild<QWidget*>("holdActivePage"));
            stackedWidget->setCurrentIndex(index);
        }

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
void MemberView::loadCatalogue() {
    //set managers
    BookManagement bookManager;
    UserManagement userManager;
    TransactionManagement transactionManager;

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
        BookListView* bookListView = bookManager.createBookList(book, entry);
        if (!bookListView) {
            qDebug() << "Failed to create BookListView for ISBN:" << isbn;
            continue;
        }

        QListWidgetItem* item = new QListWidgetItem(ui->catalogueList);

        //adding alternating colors
        if(i % 2 == 0){
            item->setBackground(QBrush(QColor(158,206,104)));
        } else {
            item->setBackground(QBrush(QColor(187,211,180)));
        }

        item->setSizeHint(bookListView->sizeHint());
        item->setData(Qt::UserRole, isbn);

        //store hidden data for transactions
        QLabel* userLabel = bookListView->findChild<QLabel*>("usernameStoredLabel");
        if(userLabel){
            userLabel->setText(username);
        }

        //adding info to display
        QLabel* activeLoanLabel = bookListView->findChild<QLabel*>("CheckedOutputLabel");
        if (activeLoanLabel) {
            activeLoanLabel->setText(transactionManager.checkedOutTo(isbn));
        }

        QLabel* holdArrayLabel = bookListView->findChild<QLabel*>("QueueOutputLabel");
        if (holdArrayLabel) {
            QJsonArray inQueue = book["inQueue"].toArray();
            holdArrayLabel->setText(QString::number(inQueue.size()));
        }

        //set availabilty display and user options
        QStackedWidget* availabilityWidget = bookListView->findChild<QStackedWidget*>("availabilityWidget");
        QStackedWidget* optionsWidget = bookListView->findChild<QStackedWidget*>("optionsStackedWidget");

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
                QWidget* availablePage = bookListView->findChild<QWidget*>("availablePage");
                QWidget* userAvailablePage = bookListView->findChild<QWidget*>("userAvailablePage");

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
                QWidget* checkedOutPage = bookListView->findChild<QWidget*>("userCheckedoutPage");
                QWidget* checkedoutPage = bookListView->findChild<QWidget*>("checkedoutPage");

                if (checkedOutPage) {
                    int index = availabilityWidget->indexOf(checkedoutPage);
                    availabilityWidget->setCurrentIndex(index);

                    index = optionsWidget->indexOf(checkedOutPage);
                    optionsWidget->setCurrentIndex(index);

                    // Populate due date
                    QLabel* dueDateLabel = bookListView->findChild<QLabel*>("dueOutputLabel");
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
                QWidget* notAvailablePage = bookListView->findChild<QWidget*>("notAvailablePage");
                QWidget* userNotAvailablePage = bookListView->findChild<QWidget*>("userNotAvailablePage");

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

        ui->catalogueList->setItemWidget(item, bookListView);

        connect(bookListView, &BookListView::refreashMemberView, this, &MemberView::updateDisplays);
    }

    qDebug() << "Catalogue loaded successfully.";
}


void MemberView::onBookClicked(QListWidgetItem *book) {

    QString isbn = book->data(Qt::UserRole).toString();

    //getting book details
    BookManagement bookManager;
    QJsonObject bookDetails = bookManager.getBookDetails(isbn);

    qDebug()<<"MemberView: Generating Book Info View";

    emit requestBookInfo(bookDetails);
}
