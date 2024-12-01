#include "viewBookItem.h"
#include "ui_viewBookItem.h"
#include "viewUpdateBook.h"
#include "viewAdminDashboard.h"
#include "managementTransaction.h"
#include "ui_viewBookItem.h"

ViewBookItem::ViewBookItem(QWidget *parent)
    : QWidget(parent),
    ui(new Ui::ViewBookItem)
{
    ui->setupUi(this);

    //connect buttons
    connect(ui->editButton, &QPushButton::clicked, this, &ViewBookItem::editBook);
    connect(ui->checkoutButton, &QPushButton::clicked, this, &ViewBookItem::checkoutBook);
    connect(ui->confirmButton, &QPushButton::clicked, this, &ViewBookItem::checkoutBook);
    connect(ui->holdButton, &QPushButton::clicked, this, &ViewBookItem::placeHold);
    connect(ui->removeHoldButton, &QPushButton::clicked, this, &ViewBookItem::removeHoldButtonClicked);
    connect(ui->removeHold02Button, &QPushButton::clicked, this, &ViewBookItem::removeHoldButtonClicked);
    connect(ui->returnButton, &QPushButton::clicked, this, &ViewBookItem::returnButtonClicked);

}

ViewBookItem::~ViewBookItem() {
    delete ui;
}

void ViewBookItem::checkoutBook() {
    managementTransaction transactionManager;
    transactionManager.checkoutBook(ui->isbnOutputLabel->text(),ui->usernameStoredLabel->text());
    qDebug()<<"viewBookItem: Emiting refreash Signal";
    emit refreashviewMemberDashboard();
}

void ViewBookItem::placeHold(){
    managementTransaction transactionManager;
    managementUser userManager;

    transactionManager.placeHold(ui->isbnOutputLabel->text(),ui->usernameStoredLabel->text());
    qDebug()<<"viewBookItem: Emiting refreash Signal";
    emit refreashView();
}

void ViewBookItem::returnButtonClicked(){
    qDebug()<<"viewBookItem: Returning book ISBN:"<<ui->isbnOutputLabel->text();
    managementTransaction transactionManager;

    transactionManager.returnBook(ui->isbnOutputLabel->text(),ui->checkedOutputLabel->text());
    emit refreashView();
}

void ViewBookItem::removeHoldButtonClicked(){
    managementTransaction transactionManager;

    if(transactionManager.removeHold(ui->holdIdOutputLabel->text())){
       emit refreashView();
    }
}

void ViewBookItem::editBook(){
    ViewUpdateBook* viewUpdateBook = new ViewUpdateBook(this);
    ViewAdminDashboard* viewAdminDashboard = qobject_cast<ViewAdminDashboard*>(parentWidget());
    qDebug()<<"parent widget is: "<<parentWidget();
    connect(viewUpdateBook, &ViewUpdateBook::updateDisplayRequest, viewAdminDashboard, &ViewAdminDashboard::updateDisplays);

    viewUpdateBook->populateDetails(ui->isbnOutputLabel->text());
    viewUpdateBook->exec();
    emit refreashView();
}

