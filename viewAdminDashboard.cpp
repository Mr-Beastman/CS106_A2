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
        QWidget *userWidget = new QWidget();
        QHBoxLayout *layout = new QHBoxLayout(userWidget);

        //adding the users details
        QLabel*nameEntryLabel = new QLabel("Name: " +user["name"].toString());
        QLabel*usernameEntryLavbel = new QLabel("Username: " +user["username"].toString());
        QLabel*accountEntryLabel = new QLabel("account #: " +user["account"].toString());
        QLabel*phoneEntryLabel = new QLabel("Phone: " +user["phone"].toString());
        QLabel*emailEntryLabel = new QLabel("Email: " +user["email"].toString());
        QLabel*activeEntryLabel = new QLabel("Active: ");

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

void ViewAdminDashboard::updateDisplays(){
    qDebug()<<"ViewAdminDashboard: Refreashing Displays";

    displayUsers();
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

//view methods
void ViewAdminDashboard::logoutButtonClicked() {
    emit logoutRequest();
}
