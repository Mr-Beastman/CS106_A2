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
}

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
        QLabel*accountEntryLabel = new QLabel("account #: " +user["account"].toString());
        QLabel*phoneEntryLabel = new QLabel("Phone: " +user["phone"].toString());
        QLabel*emailEntryLabel = new QLabel("Email: " +user["email"].toString());

        //display isActive Status
        QLabel*isActiveLabel = new QLabel();
        QIcon statusIcon = user["isActive"].toBool()
            ? style()->standardIcon(QStyle::SP_DialogApplyButton)
            : style()->standardIcon(QStyle::SP_DialogCancelButton);
        isActiveLabel->setPixmap(statusIcon.pixmap(16,16));


        //adding widgest to layout
        layout->addWidget(nameEntryLabel);
        layout->addWidget(accountEntryLabel);
        layout->addWidget(phoneEntryLabel);
        layout->addWidget(emailEntryLabel);
        layout->addWidget(isActiveLabel);


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

    qDebug()<<"viewAdminDashboard: User Database Populated";
}

void ViewAdminDashboard::updateDisplays(){
    qDebug()<<"viewAdminDashboard: Refreashing Displays";

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


        QStackedWidget* stackedWidget = viewBookItem->findChild<QStackedWidget*>("optionsStackedWidget");
        int index = stackedWidget->indexOf(viewBookItem->findChild<QWidget*>("adminPage"));
        stackedWidget->setCurrentIndex(index);

        //getting current book activity.
        QLabel* activeLoan  =  viewBookItem->findChild<QLabel*>("checkedOutputLabel");
        activeLoan->setText(transactionManager.checkedOutTo(book["isbn"].toString()));

        QJsonArray queue = book["inQueue"].toArray();

        QLabel* holdArray =  viewBookItem->findChild<QLabel*>("QueueOutputLabel");
        if(queue.isEmpty()){
            holdArray->setText(QString::number(book["inQueue"].toArray().size()));
        } else {
            holdArray->setText(QString::number(0));
        }


        //enabling/disabling return button depending on status
        QPushButton* returnButton = viewBookItem->findChild<QPushButton*>("returnButton");
        stackedWidget = viewBookItem->findChild<QStackedWidget*>("availabilityWidget");

        if(book["isAvailable"].toBool()){
            returnButton->hide();
            index = stackedWidget->indexOf(viewBookItem->findChild<QWidget*>("availablePage"));
            stackedWidget->setCurrentIndex(index);
        } else {
            if(!queue.isEmpty()){
                QJsonObject firstInQueue = queue[0].toObject();
                QString holdStatus = firstInQueue["holdStatus"].toString();

                if(holdStatus == "ready"){
                    returnButton->hide();
                    index = stackedWidget->indexOf(viewBookItem->findChild<QWidget*>("holdReadyPage"));
                    stackedWidget->setCurrentIndex(index);
                } else if (holdStatus == "active"){
                    returnButton->show();
                    index = stackedWidget->indexOf(viewBookItem->findChild<QWidget*>("notAvailablePage"));
                    stackedWidget->setCurrentIndex(index);
                }
            } else {
                returnButton->show();
                index = stackedWidget->indexOf(viewBookItem->findChild<QWidget*>("notAvailablePage"));
                stackedWidget->setCurrentIndex(index);
            }
        }

        QLabel* queueLabel = viewBookItem->findChild<QLabel*>("QueueOutputLabel");
        queueLabel->setText(QString::number(queue.size()));

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

    qDebug()<<"viewAdminDashboard: Requesting to display member info view";
    emit requestMemberInfo(toView);
}

void ViewAdminDashboard::onBookClicked(QListWidgetItem *book) {
    //getting the assigned isbn from clicked item
    QString isbn = book->data(Qt::UserRole).toString();

    //getting book details
    ManagementBook bookManger;
    QJsonObject bookDetails = bookManger.getBookDetails(isbn);

    qDebug()<<"viewMemberDashboard: Generating Book Info View";

    emit requestBookInfo(bookDetails);
}

//view methods
void ViewAdminDashboard::logoutButtonClicked() {
    emit logoutRequest();
}
