#include "adminView.h"
#include "ui_adminView.h"
#include "addBookview.h"
#include "userManagement.h"
#include "bookManagement.h"
#include "transactionManagement.h"
#include "bookListView.h"

AdminView::AdminView(QWidget *parent) : QWidget(parent), ui(new Ui::AdminView) {
    ui->setupUi(this);

    //connect buttons
    connect(ui->addButton, &QPushButton::clicked, this, &AdminView::addButtonClicked);
    connect(ui->logoutButton, &QPushButton::clicked, this, &AdminView::logoutButtonClicked);


    //connect signals
    // ModifyBookView* modifyBookPage = new ModifyBookView(this);
    // connect(modifyBookPage, &ModifyBookView::requestRefreash, this, &AdminView::updateDisplays);
}

void AdminView::displayUsers() {

    UserManagement* userManager = UserManagement::getUserManager();

    if(userManager == nullptr){
        qDebug()<<"AdminView: UserManager is Null";
        return;
    }

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

        //adding widgest to layout
        layout->addWidget(nameEntryLabel);
        layout->addWidget(accountEntryLabel);
        layout->addWidget(phoneEntryLabel);
        layout->addWidget(emailEntryLabel);


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
        entry->setData(Qt::UserRole, user["account"].toString());

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
    connect(&addBookDialog, &AddBookView::updateDisplayRequest, this, &AdminView::updateDisplays);
    addBookDialog.exec();
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
