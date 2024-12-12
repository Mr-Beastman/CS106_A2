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
    connect(ui->confirmButton, &QPushButton::clicked, this, &ViewBookItem::confirmButtonClicked);
    connect(ui->holdButton, &QPushButton::clicked, this, &ViewBookItem::placeHold);
    connect(ui->removeHoldButton, &QPushButton::clicked, this, &ViewBookItem::removeHoldButtonClicked);
    connect(ui->removeHold02Button, &QPushButton::clicked, this, &ViewBookItem::removeHoldButtonClicked);
    connect(ui->returnButton, &QPushButton::clicked, this, &ViewBookItem::returnButtonClicked);
}

ViewBookItem::~ViewBookItem() {
    delete ui;
}

void ViewBookItem::checkoutBook() {
    ManagementTransaction transactionManager;
    transactionManager.checkoutBook(ui->isbnOutputLabel->text(),ui->usernameStoredLabel->text());
    qDebug()<<"viewBookItem: Emiting refreash Signal";
    emit refreashviewMemberDashboard();
}

void ViewBookItem::placeHold(){
    ManagementTransaction transactionManager;
    ManagementUser userManager;

    transactionManager.placeHold(ui->isbnOutputLabel->text(),ui->usernameStoredLabel->text());
    qDebug()<<"viewBookItem: Emiting refreash Signal";
    emit refreashviewMemberDashboard();
}

void ViewBookItem::returnButtonClicked(){
    qDebug()<<"viewBookItem: Returning book ISBN:"<<ui->isbnOutputLabel->text();
    //return book
    ManagementTransaction transactionManager;
    transactionManager.returnBook(ui->isbnOutputLabel->text(),ui->checkedOutputLabel->text());

    //set next in queues hold status to ready
    ManagementBook bookManager;
    if(!bookManager.isAvailable(ui->isbnOutputLabel->text()) && !bookManager.isIssued(ui->isbnOutputLabel->text())) {
        transactionManager.notifyNextInQueue(ui->isbnOutputLabel->text());
        qDebug()<<"ViewBookItem: Next user in Queue has been notified";
    }
    emit refreashView();
}

void ViewBookItem::removeHoldButtonClicked(){\
    //removing hold
    ManagementTransaction transactionManager;
    transactionManager.removeHold(ui->holdStoredIdLabel->text());

    //notifying next in que if current user declined to pick up
    ManagementBook bookManager;
    if(!bookManager.isAvailable(ui->isbnOutputLabel->text()) && !bookManager.isIssued(ui->isbnOutputLabel->text())) {
        if(transactionManager.notifyNextInQueue(ui->isbnOutputLabel->text())){
            qDebug()<<"ViewBookItem: Next user in Queue has been notified";
        } else {
            qDebug()<<"ViewBookItem: Failed to notify next in queue";
        }
    }
    qDebug()<<"ViewBookItem: Requesting view Refreash";
    emit refreashviewMemberDashboard();
}

void ViewBookItem::confirmButtonClicked(){
    ManagementTransaction transactionManager;
    ManagementData dataManager;

    transactionManager.checkoutBook(ui->isbnOutputLabel->text(),ui->usernameStoredLabel->text());
    transactionManager.removeHold(ui->holdStoredIdLabel->text());
    emit refreashviewMemberDashboard();
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

