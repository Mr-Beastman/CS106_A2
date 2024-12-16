#include "viewAdminDashboard.h"
#include "ui_viewAdminDashboard.h"
#include "viewAddBook.h"
#include "viewAddMember.h"
#include "managementUser.h"
#include "managementBook.h"
#include "managementTransaction.h"
#include "viewBookItem.h"

ViewAdminDashboard::ViewAdminDashboard(QWidget *parent) : QWidget(parent), ui(new Ui::ViewAdminDashboard) {
    ui->setupUi(this);

    //connect buttons
    connect(ui->addButton, &QPushButton::clicked, this, &ViewAdminDashboard::addButtonClicked);
    connect(ui->addMemberButton, &QPushButton::clicked, this, &ViewAdminDashboard::addMemberButtonClicked);
    connect(ui->logoutButton, &QPushButton::clicked, this, &ViewAdminDashboard::logoutButtonClicked);

    //connect list items clicked
    connect(ui->membersList, &QListWidget::itemClicked, this, &ViewAdminDashboard::onMemberClicked);
    connect(ui->newMemberList, &QListWidget::itemClicked, this, &ViewAdminDashboard::onMemberClicked);
    connect(ui->catalogueList, &QListWidget::itemClicked, this, &ViewAdminDashboard::onBookClicked);
}

void ViewAdminDashboard::setAdminUser(const QString& username){
    adminUser = username;
}

 QString ViewAdminDashboard::getAdminUser(){
    return adminUser;
}

//dyanmically populate users list from the Json file ["users"]
//parameters: none
//returns: none
void ViewAdminDashboard::displayUsers() {

    ManagementUser userManager;

    ui->membersList->clear();

    QJsonArray toDisplay = userManager.getUserArray();

    for(int i = 0; i<toDisplay.size(); ++i){
        QJsonObject user = toDisplay[i].toObject();

        //creating widget to hold user detials
        QWidget* userWidget = createUserEntry(user);

        QListWidgetItem *entry = new QListWidgetItem();
        entry->setSizeHint(userWidget->sizeHint());

        //adding alternating colors
        if(i % 2 == 0){
            entry->setBackground(QBrush(QColor(158,206,104)));
        } else {
            entry->setBackground(QBrush(QColor(187,211,180)));
        }

        //adding entry widget to the ui file
        ui->membersList->addItem(entry);

        //assigning account# to entry
        entry->setData(Qt::UserRole, user["username"].toString());

        ui->membersList->setItemWidget(entry, userWidget);
    }

    qDebug()<<"ViewAdminDashboard: User Database Populated";
}

void ViewAdminDashboard::displayNewUsers(){
    ManagementUser userManager;


    ui->newMemberList->clear();

    QJsonArray toDisplay = userManager.getUserArray();

    //flag for stacked widget display options
    bool newUsers = false;

    for(int i = 0; i<toDisplay.size(); ++i){
        QJsonObject user = toDisplay[i].toObject();

        if(!userManager.isActive(user["username"].toString())){
            newUsers = true;
            //creating widget to hold user detials
            QWidget* userWidget = createUserEntry(user);

            QListWidgetItem *entry = new QListWidgetItem();
            entry->setSizeHint(userWidget->sizeHint());

            //adding alternating colors
            if(i % 2 == 0){
                entry->setBackground(QBrush(QColor(158,206,104)));
            } else {
                entry->setBackground(QBrush(QColor(187,211,180)));
            }

            //adding entry widget to the ui file
            ui->newMemberList->addItem(entry);

            //assigning account# to entry
            entry->setData(Qt::UserRole, user["username"].toString());

            ui->newMemberList->setItemWidget(entry, userWidget);
        }
    }

    //set stacked widget based on earlier flag
    if(newUsers){
        ui->newMemberStackedWidget->setCurrentIndex(0);
        qDebug()<<"ViewAdminDashboard: New user report populated";
        ui->newMemberList->show();
    } else {
        ui->newMemberStackedWidget->setCurrentIndex(1);
        qDebug()<<"ViewAdminDashboard: No new users to report";
        ui->newMemberList->hide();
    }
}

void ViewAdminDashboard::updateDisplays(){
    qDebug()<<"ViewAdminDashboard: Refreashing Displays";

    displayUsers();
    displayNewUsers();
    displayAdminCatalogue();
}

//manage catalogue functions
void ViewAdminDashboard::addButtonClicked(){
    ViewAddBook addBookDialog(this);
    addBookDialog.exec();
    updateDisplays();
}

void ViewAdminDashboard::addMemberButtonClicked(){
    ViewAddMember addMemberDialog(this);
    addMemberDialog.exec();
    updateDisplays();
}

void ViewAdminDashboard::displayAdminCatalogue(){
    //getting managers
    ManagementData dataManager;
    ManagementBook bookManager;
    ManagementTransaction transactionManager;

    //get data arrays
    QJsonArray bookData = bookManager.getBookArray();

    //clear list
    ui->catalogueList->clear();

    //looping throigh books to create a entry for it
    for(int i = 0; i<bookData.size(); ++i){
        QJsonObject book = bookData[i].toObject();


        QJsonObject entry;
        ViewBookItem* viewBookItem = bookManager.createBookList(book, entry);

        //adding entry to list
        QListWidgetItem* item = new QListWidgetItem(ui->catalogueList);


        //adding alternating colors
        if(i % 2 == 0){
            item->setBackground(QBrush(QColor(158,206,104)));
        } else {
            item->setBackground(QBrush(QColor(187,211,180)));
        }


        transactionManager.setBookAvailibityOptions(viewBookItem, book, adminUser);

        item->setSizeHint(viewBookItem->sizeHint());

        item->setData(Qt::UserRole, book["isbn"].toString());

        ui->catalogueList->setItemWidget(item, viewBookItem);

        connect(viewBookItem, &ViewBookItem::refreashView, this, &ViewAdminDashboard::updateDisplays);
    }
}

void ViewAdminDashboard::displayOverdueBooks(){
    //can use latest child transaction manager to access all inherited methods
    ManagementTransaction transactionManager;

    //get data arrays
    QJsonArray bookData = transactionManager.getBookArray();
    QJsonArray userArray = transactionManager.getUserArray();

    //clear list
    ui->overdueList->clear();

    bool overdueBooks = false;

    //looping through books to create an entry for it
    for (int i = 0; i < bookData.size(); ++i) {
        QJsonObject book = bookData[i].toObject();

        //only check books that are issued
        if (!book["issuedTo"].toString().isEmpty()) {
            qDebug()<<"=======Checking "<<book["title"].isString();
            //looping through users to find the user who has the book issued
            for (int j = 0; j < userArray.size(); ++j) {
                QJsonObject user = userArray[j].toObject();

                //check if the book is issued to this user
                if (book["issuedTo"].toString() == user["username"].toString()) {
                    //check if the user has active loans
                    QJsonArray activeLoans = user["activeLoans"].toArray();
                    for (int k = 0; k < activeLoans.size(); ++k) {
                        QJsonObject loan = activeLoans[k].toObject();

                        //check if the loan corresponds to the current book and if it's overdue
                        if (loan["isbn"].toString() == book["isbn"].toString()) {
                            if (loan["status"].toString() == "overdue") {
                                //create a new entry for this overdue book
                                QJsonObject entry;
                                ViewBookItem* viewBookItem = transactionManager.createBookList(book, entry);

                                //hardcoded to overwrite display options
                                QStackedWidget* availabilityWidget = viewBookItem->findChild<QStackedWidget*>("availabilityWidget");
                                QStackedWidget* optionsWidget = viewBookItem->findChild<QStackedWidget*>("optionsStackedWidget");

                                QWidget* availabilityPage = viewBookItem->findChild<QWidget*>("overduePage");
                                QWidget* optionsPage = viewBookItem->findChild<QWidget*>("adminPage");
                                int index = 0;

                                QLabel* requestLabel = viewBookItem->findChild<QLabel*>("requestLabel");
                                QLabel* requestDisplayLabel = viewBookItem->findChild<QLabel*>("requestOutputLabel");
                                QLabel* checkedoutLabel = viewBookItem->findChild<QLabel*>("checkedOutputLabel");

                                QLabel* queueLabel = viewBookItem->findChild<QLabel*>("QueueLabel");
                                QLabel* queueOutputLabel = viewBookItem->findChild<QLabel*>("QueueOutputLabel");

                                if(checkedoutLabel){
                                    checkedoutLabel->setText(book["issuedTo"].toString());
                                }

                                if(queueLabel && queueOutputLabel){
                                    queueLabel->hide();
                                    queueOutputLabel->hide();
                                }

                                if(requestLabel && requestDisplayLabel){
                                    requestLabel->hide();
                                    requestDisplayLabel->hide();
                                }

                                if (availabilityPage) {
                                    index = availabilityWidget->indexOf(availabilityPage);
                                    availabilityWidget->setCurrentIndex(index);
                                }
                                if (optionsPage) {
                                    index = optionsWidget->indexOf(optionsPage);
                                    optionsWidget->setCurrentIndex(index);
                                }

                                //adding entry to the list
                                QListWidgetItem* item = new QListWidgetItem(ui->overdueList);

                                //adding alternating colors
                                if (i % 2 == 0) {
                                    item->setBackground(QBrush(QColor(158, 206, 104)));
                                } else {
                                    item->setBackground(QBrush(QColor(187, 211, 180)));
                                }

                                item->setSizeHint(viewBookItem->sizeHint());
                                item->setData(Qt::UserRole, book["isbn"].toString());

                                ui->overdueList->setItemWidget(item, viewBookItem);

                                connect(viewBookItem, &ViewBookItem::refreashView, this, &ViewAdminDashboard::updateDisplays);
                                overdueBooks = true;
                            }
                        }
                    }
                }
            }
        }
    }

    if(overdueBooks){
        ui->overduetackedWidget->setCurrentIndex(0);
        qDebug()<<"ViewAdminDashboard: New user report populated";
    } else {
        ui->overduetackedWidget->setCurrentIndex(1);
        qDebug()<<"ViewAdminDashboard: No new users to report";
    }
}


void ViewAdminDashboard::onMemberClicked(QListWidgetItem *user){
    QString userAccount = user->data(Qt::UserRole).toString();

    //getMemberDetails
    ManagementUser userManager;

    qDebug()<<"viewAdminDashboard: Populating Member Info View";
    QJsonObject toView = userManager.getUserObj(userAccount);

    QString adminAccount = userManager.getAccount(adminUser);

    qDebug()<<"viewAdminDashboard: Requesting to display member info view";
    emit requestMemberInfo(toView, adminAccount);
}

void ViewAdminDashboard::onBookClicked(QListWidgetItem *book) {
    //getting the assigned isbn from clicked item
    QString isbn = book->data(Qt::UserRole).toString();

    //getting book details
    ManagementBook bookManger;
    QJsonObject bookDetails = bookManger.getBookDetails(isbn);

    qDebug()<<"ViewAdminDashboard: Generating Book Info View";
    emit requestBookInfo(bookDetails,adminUser);
}

QWidget *ViewAdminDashboard::createUserEntry(const QJsonObject &user){
    //creating widget to hold user detials
    QWidget *userWidget = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(userWidget);

    //adding the users details
    QLabel*nameEntryLabel = new QLabel("Name: " +user["name"].toString());
    QLabel*usernameEntryLavbel = new QLabel("Username: " +user["username"].toString());
    QLabel*accountEntryLabel = new QLabel("Account #: " +user["account"].toString());
    QLabel*phoneEntryLabel = new QLabel("Phone: " +user["phone"].toString());
    QLabel*emailEntryLabel = new QLabel("Email: " +user["email"].toString());
    QLabel*activeEntryLabel = new QLabel("Active: ");

    //forcing black font color as they show white otherwise dwespite stylesheet settings
    QList<QLabel*> labels = {nameEntryLabel, usernameEntryLavbel, accountEntryLabel, phoneEntryLabel, emailEntryLabel, activeEntryLabel};

    for(int i = 0; i<labels.size(); i++){
        labels[i]->setStyleSheet("color: black; font: 700 9pt 'Segoe UI';");
    }

    //display isActive Status
    QLabel*isActiveLabel = new QLabel();
    QIcon statusIcon = user["isActive"].toBool()
                           ? style()->standardIcon(QStyle::SP_DialogApplyButton)
                           : style()->standardIcon(QStyle::SP_DialogCancelButton);
    isActiveLabel->setPixmap(statusIcon.pixmap(16,16));


    //adding widgest to layout
    layout->addWidget(nameEntryLabel);
    layout->addWidget(usernameEntryLavbel);
    layout->addWidget(accountEntryLabel);
    layout->addWidget(phoneEntryLabel);
    layout->addWidget(emailEntryLabel);
    layout->addWidget(activeEntryLabel);
    layout->addWidget(isActiveLabel);

    //setting stretchFactors
    layout->setStretch(0,1);
    layout->setStretch(1,1);
    layout->setStretch(2,1);
    layout->setStretch(3,1);
    layout->setStretch(4,1);
    layout->setStretch(5,0);
    layout->setStretch(6,1);

    return userWidget;
}

//view methods
void ViewAdminDashboard::logoutButtonClicked() {
    emit logoutRequest();
}
