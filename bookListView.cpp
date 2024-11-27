#include "BookListView.h"
#include "ui_BookListView.h"
#include "transactionManagement.h"

BookListView::BookListView(QWidget *parent)
    : QWidget(parent),
    ui(new Ui::BookListView)
{
    ui->setupUi(this);

    //connect buttons
    connect(ui->checkoutButton, &QPushButton::clicked, this, &BookListView::checkoutBook);
    connect(ui->holdButton, &QPushButton::clicked, this, &BookListView::placeHold);

}

BookListView::~BookListView() {
    delete ui;
}

void BookListView::checkoutBook() {
    TransactionManagement* manager = TransactionManagement::getTransactionManager();
    UserManagement* userManager = UserManagement::getUserManager();

    QJsonObject& user = userManager->getCurrentUser();

    qDebug()<<user;

    manager->checkoutBook(ui->isbnOutputLabel->text());
    qDebug()<<"BookListView: Emiting refreash Signal";
    emit refreashView();
}

void BookListView::placeHold(){
    TransactionManagement* manager = TransactionManagement::getTransactionManager();
    UserManagement* userManager = UserManagement::getUserManager();

    QJsonObject user = userManager->getCurrentUser();

    manager->placeHold(user["username"].toString(),ui->isbnOutputLabel->text());
    qDebug()<<"BookListView: Emiting refreash Signal";
    emit refreashView();
}

