#include "BookListView.h"
#include "ui_BookListView.h"
#include "modifyBookview.h"
#include "adminView.h"
#include "transactionManagement.h"
#include "ui_bookListView.h"

BookListView::BookListView(QWidget *parent)
    : QWidget(parent),
    ui(new Ui::BookListView)
{
    ui->setupUi(this);

    //connect buttons
    connect(ui->editButton, &QPushButton::clicked, this, &BookListView::editBook);
    connect(ui->checkoutButton, &QPushButton::clicked, this, &BookListView::checkoutBook);
    connect(ui->confirmButton, &QPushButton::clicked, this, &BookListView::checkoutBook);
    connect(ui->holdButton, &QPushButton::clicked, this, &BookListView::placeHold);
    connect(ui->removeHoldButton, &QPushButton::clicked, this, &BookListView::removeHoldButtonClicked);
    connect(ui->removeHold02Button, &QPushButton::clicked, this, &BookListView::removeHoldButtonClicked);
    connect(ui->returnButton, &QPushButton::clicked, this, &BookListView::returnButtonClicked);

}

BookListView::~BookListView() {
    delete ui;
}

void BookListView::checkoutBook() {
    TransactionManagement transactionManager;
    transactionManager.checkoutBook(ui->isbnOutputLabel->text(),ui->usernameStoredLabel->text());
    qDebug()<<"BookListView: Emiting refreash Signal";
    emit refreashMemberView();
}

void BookListView::placeHold(){
    TransactionManagement transactionManager;
    UserManagement userManager;

    transactionManager.placeHold(ui->isbnOutputLabel->text(),ui->usernameStoredLabel->text());
    qDebug()<<"BookListView: Emiting refreash Signal";
    emit refreashView();
}

void BookListView::returnButtonClicked(){
    qDebug()<<"BookListView: Returning book ISBN:"<<ui->isbnOutputLabel->text();
    TransactionManagement transactionManager;

    transactionManager.returnBook(ui->isbnOutputLabel->text(),ui->checkedOutputLabel->text());
    emit refreashView();
}

void BookListView::removeHoldButtonClicked(){
    TransactionManagement transactionManager;

    if(transactionManager.removeHold(ui->holdIdOutputLabel->text())){
       emit refreashView();
    }
}

void BookListView::editBook(){
    ModifyBookView* modifyBookView = new ModifyBookView(this);
    AdminView* adminView = qobject_cast<AdminView*>(parentWidget());
    qDebug()<<"parent widget is: "<<parentWidget();
    connect(modifyBookView, &ModifyBookView::updateDisplayRequest, adminView, &AdminView::updateDisplays);

    modifyBookView->populateDetails(ui->isbnOutputLabel->text());
    modifyBookView->exec();
    emit refreashView();
}

