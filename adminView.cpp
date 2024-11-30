#include "adminView.h"
#include "ui_adminView.h"
#include "addBookview.h"
#include "addMemberView.h"
#include "userManagement.h"
#include "bookManagement.h"
#include "transactionManagement.h"
#include "memberInfoView.h"
#include "bookListView.h"

AdminView::AdminView(QWidget *parent) : QWidget(parent), ui(new Ui::AdminView) {
    ui->setupUi(this);

    //connect buttons
    connect(ui->addButton, &QPushButton::clicked, this, &AdminView::addButtonClicked);
    connect(ui->addMemberButton, &QPushButton::clicked, this, &AdminView::addMemberButtonClicked);
    connect(ui->logoutButton, &QPushButton::clicked, this, &AdminView::logoutButtonClicked);

    //connect list items clicked
    connect(ui->membersList, &QListWidget::itemClicked, this, &AdminView::onMemberClicked);
}

void AdminView::displayUsers() {

    UserManagement* userManager = UserManagement::getUserManager();

    if(userManager == nullptr){
        qDebug()<<"AdminView: UserManager is Null";
        return;
    }

    ui->membersList->clear();

    QJsonArray& toDisplay = userManager->getUserArray();

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

    qDebug()<<"AdminView: User Database Populated";
}

void AdminView::updateDisplays(){
    qDebug()<<"AdminView: Refreashing Displays";

    displayUsers();
    loadAdminCatalogue();
}

//manage catalogue functions
void AdminView::addButtonClicked(){
    AddBookView addBookDialog(this);
    addBookDialog.exec();
    updateDisplays();
}

void AdminView::addMemberButtonClicked(){
    AddMemberView addMemberDialog(this);
    addMemberDialog.exec();
    updateDisplays();
}

void AdminView::loadAdminCatalogue(){

    BookManagement *bookManager = BookManagement::getBookManager();
    TransactionManagement* transactionManager = TransactionManagement::getTransactionManager();

    QJsonArray& bookData = bookManager->getBookArray();

    //clear list
    ui->catalogueList->clear();

    //looping throigh books to create a entry for it
    for(int i = 0; i<bookData.size(); ++i){
        QJsonObject book = bookData[i].toObject();

        QJsonObject entry;
        BookListView* bookListView = bookManager->createBookList(book, entry);

        //adding entry to list
        QListWidgetItem* item = new QListWidgetItem(ui->catalogueList);

        //adding alternating colors
        if(i % 2 == 0){
            item->setBackground(QBrush(QColor(158,206,104)));
        } else {
            item->setBackground(QBrush(QColor(187,211,180)));
        }

        QStackedWidget* stackedWidget = bookListView->findChild<QStackedWidget*>("optionsStackedWidget");
        int index = stackedWidget->indexOf(bookListView->findChild<QWidget*>("adminPage"));
        stackedWidget->setCurrentIndex(index);

        QLabel* checkedOut =  bookListView->findChild<QLabel*>("CheckedOutputLabel");
        checkedOut->setText(transactionManager->checkedOutTo(book["isbn"].toString()));

        item->setSizeHint(bookListView->sizeHint());

        item->setData(Qt::UserRole, book["isbn"].toString());

        ui->catalogueList->setItemWidget(item, bookListView);

        connect(bookListView, &BookListView::refreashView, this, &AdminView::updateDisplays);
    }
}

void AdminView::onMemberClicked(QListWidgetItem *user){
    QString userAccount = user->data(Qt::UserRole).toString();

    //getMemberDetails
    UserManagement* userManager = UserManagement::getUserManager();

    qDebug()<<"AdminView: Populating Member Info View";
    QJsonObject toView = userManager->getUserObj(userAccount);

    qDebug()<<"AdminView: Requesting to display member info view";
    emit requestMemberInfo(toView);
}

void AdminView::onBookClicked(QListWidgetItem *book) {
    //getting the assigned isbn from clicked item
    QString isbn = book->data(Qt::UserRole).toString();

    //getting book details
    BookManagement* bookManger = BookManagement::getBookManager();
    QJsonObject bookDetails = bookManger->getBookDetails(isbn);

    qDebug()<<"MemberView: Generating Book Info View";

    emit requestBookInfo(bookDetails);
}

//view methods
void AdminView::logoutButtonClicked() {
    emit logoutRequest();
}
