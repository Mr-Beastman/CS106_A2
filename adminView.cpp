#include "adminView.h"
#include "ui_adminView.h"
#include "addBookview.h"
#include "userManagement.h"
#include "bookManagement.h"
#include "bookListView.h"

AdminView::AdminView(QWidget *parent) : QWidget(parent), ui(new Ui::AdminView) {
    ui->setupUi(this);

    //set up for manage catalogue
    connect(ui->addButton, &QPushButton::clicked, this, &AdminView::addButtonClicked);
    connect(ui->logoutButton, &QPushButton::clicked, this, &AdminView::logoutButtonClicked);
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

    QJsonArray& bookData = bookManager->getBookArray();

    //clear list
    ui->catalogueList->clear();

    //looping throigh books to create a entry for it
    for(int i = 0; i<bookData.size(); ++i){
        QJsonObject book = bookData[i].toObject();

        BookListView* bookListView = new BookListView(ui->catalogueList);

        // // Adding data to the entry
        QLabel* titleLabel = bookListView->findChild<QLabel*>("titleOutputLabel");
        titleLabel->setText(book["title"].toString());

        QLabel* authorLabel = bookListView->findChild<QLabel*>("authorOutputLabel");
        authorLabel->setText(book["author"].toString());

        QLabel* isbnLabel = bookListView->findChild<QLabel*>("isbnOutputLabel");
        isbnLabel->setText(book["isbn"].toString());

        // adding cover image
        QLabel* coverLabel = bookListView->findChild<QLabel*>("coverLabel");
        if (coverLabel) {
            QString coverImagePath = bookManager->findCoverPath() + book["isbn"].toString() + ".png";
            QPixmap cover(coverImagePath);
            QString noCoverImagePath = bookManager->findCoverPath() + "noCover.png";
            QPixmap noCover(noCoverImagePath);

            if (!cover.isNull()) {
                coverLabel->setPixmap(cover);
            } else {
                coverLabel->setPixmap(noCover);
            }
            coverLabel->setScaledContents(true);
        } else {
            qDebug() << "MemberView: Cover label not found";
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
        ui->catalogueList->setItemWidget(item, bookListView);
    }
}


//view methods
void AdminView::logoutButtonClicked() {
    emit logoutRequest();
}
