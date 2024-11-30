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
    connect(ui->removeHold02Button, &QPushButton::clicked, this, &BookListView::removeHoldButtonClicked);
    connect(ui->returnButton, &QPushButton::clicked, this, &BookListView::returnButtonClicked);

}

BookListView::~BookListView() {
    delete ui;
}

void BookListView::checkoutBook() {
    TransactionManagement* transactionManager = TransactionManagement::getTransactionManager();
    transactionManager->checkoutBook(ui->isbnOutputLabel->text());
    qDebug()<<"BookListView: Emiting refreash Signal";
    emit refreashMemberView();
}

void BookListView::placeHold(){
    TransactionManagement* manager = TransactionManagement::getTransactionManager();
    UserManagement* userManager = UserManagement::getUserManager();

    QJsonObject user = userManager->getCurrentUser();

    manager->placeHold(ui->isbnOutputLabel->text());
    qDebug()<<"BookListView: Emiting refreash Signal";
    emit refreashView();
}

void BookListView::returnButtonClicked(){
    TransactionManagement* transactionManager = TransactionManagement::getTransactionManager();
    transactionManager->returnBook(ui->isbnOutputLabel->text());
    emit refreashView();
}

void BookListView::removeHoldButtonClicked(){
    TransactionManagement* transactionManager = TransactionManagement::getTransactionManager();
    transactionManager->removeHold(ui->isbnOutputLabel->text());
    emit refreashView();
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

