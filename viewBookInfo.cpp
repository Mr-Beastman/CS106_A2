#include "viewBookInfo.h"
#include "managementBook.h"
#include "managementTransaction.h"
#include "ui_viewBookInfo.h"
#include "viewUpdateBook.h"




ViewBookInfo::ViewBookInfo(QWidget* parent) : QDialog(parent), ui(new Ui::ViewBookInfo) {
    ui->setupUi(this);

    //connect buttons
    connect(ui->backButton, &QPushButton::clicked, this, &ViewBookInfo::backButtonClicked);
    connect(ui->logoutButton, &QPushButton::clicked, this, &::ViewBookInfo::logoutButtonClicked);
    connect(ui->checkoutButton, &QPushButton::clicked, this, &::ViewBookInfo::checkoutButtonClicked);
    connect(ui->holdButton_2, &QPushButton::clicked, this, &::ViewBookInfo::holdButtonClicked);
    connect(ui->removeHoldButton, &QPushButton::clicked, this, &::ViewBookInfo::removeHoldbuttonClicked);
    connect(ui->removeHold02Button, &QPushButton::clicked, this, &::ViewBookInfo::removeHoldbuttonClicked);
    connect(ui->confirmButton, &QPushButton::clicked, this, &::ViewBookInfo::confirmButtonClicked);
    connect(ui->returnButton, &QPushButton::clicked, this, &::ViewBookInfo::returnButtonClicked);
    connect(ui->editButton, &QPushButton::clicked, this, &::ViewBookInfo::editButtonClicked);
}

ViewBookInfo::~ViewBookInfo() {
    delete ui;
}

void ViewBookInfo::backButtonClicked() {
    emit goBack();
}

void ViewBookInfo::checkoutButtonClicked(){
    ManagementTransaction transactionManager;
    transactionManager.checkoutBook(ui->isbnOutputLabel->text(), currentUser);
    refreashBookInfo();
}

void ViewBookInfo::returnButtonClicked(){
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
    refreashBookInfo();
}

void ViewBookInfo::holdButtonClicked(){
    ManagementTransaction transactionManager;
    transactionManager.placeHold(ui->isbnOutputLabel->text(),currentUser);
    refreashBookInfo();
}

void ViewBookInfo::removeHoldbuttonClicked(){
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
    refreashBookInfo();
}

void ViewBookInfo::confirmButtonClicked(){
    ManagementTransaction transactionManager;

    transactionManager.checkoutBook(ui->isbnOutputLabel->text(),currentUser);
    transactionManager.removeHold(ui->holdStoredIdLabel->text());
    refreashBookInfo();
}

void ViewBookInfo::editButtonClicked(){
    ViewUpdateBook* viewUpdateBook = new ViewUpdateBook(this);
    ViewBookInfo* viewBookInfo = qobject_cast<ViewBookInfo*>(parentWidget());
    qDebug()<<"parent widget is: "<<parentWidget();
    connect(viewUpdateBook, &ViewUpdateBook::updateDisplayRequest, viewBookInfo, &ViewBookInfo::refreashBookInfo);

    viewUpdateBook->populateDetails(ui->isbnOutputLabel->text());
    viewUpdateBook->exec();
    refreashBookInfo();
}

void ViewBookInfo::refreashBookInfo(){
    ManagementBook bookManager;
    QJsonObject book = bookManager.getBookDetails(ui->isbnOutputLabel->text());

    setBookDetails(book);
    setBookAvailibity(book, currentUser);

    emit refreashMemberDisplay();
}

void ViewBookInfo::setBookDetails(const QJsonObject &bookDetails){

    ui->titleLabel->setText(bookDetails["title"].toString());
    ui->authorOutputLabel->setText(bookDetails["author"].toString());
    ui->isbnOutputLabel->setText(bookDetails["isbn"].toString());
    ui->bookDescription->setPlainText(bookDetails["desc"].toString());

    // adding cover image
    ManagementBook bookManager;
    QString coverImagePath = bookManager.findCoverPath() + bookDetails["isbn"].toString() + ".png";
    QPixmap cover(coverImagePath);
    QString noCoverImagePath = bookManager.findCoverPath() + "noCover.png";
    QPixmap noCover(noCoverImagePath);

    if (!cover.isNull()) {
        ui->coverLabel->setPixmap(cover);
    } else {
        ui->coverLabel->setPixmap(noCover);
    }

    qDebug()<<"viewBookInfo: Book Info View Generated";
}

void ViewBookInfo::setBookAvailibity(const QJsonObject& book, const QString& username){

    qDebug()<<"ViewBookInfo: Setting Availbility and Options";
    qDebug()<<book["title"].toString();
    qDebug()<<username;

    ManagementTransaction transactionManager;
    qDebug()<<currentUser;
    transactionManager.setBookAvailibityOptions(this, book, currentUser);
}

void ViewBookInfo::logoutButtonClicked() {
    emit logoutRequest();
}

void ViewBookInfo::hideAdminInfo(){
    ui->adminInfoDisplay->hide();
}

void ViewBookInfo::showAdminInfo(){
    ui->adminInfoDisplay->show();
}

void ViewBookInfo::setCurrentUser(const QString &username){
    currentUser = username;
}
