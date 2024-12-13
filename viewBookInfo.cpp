#include "viewBookInfo.h"
#include "managementBook.h"
#include "managementTransaction.h"
#include "ui_viewBookInfo.h"




ViewBookInfo::ViewBookInfo(QWidget* parent) : QDialog(parent), ui(new Ui::ViewBookInfo) {
    ui->setupUi(this);

    //connect buttons
    connect(ui->backButton, &QPushButton::clicked, this, &ViewBookInfo::backButtonClicked);
    connect(ui->logoutButton, &QPushButton::clicked, this, &::ViewBookInfo::logoutButtonClicked);
}

ViewBookInfo::~ViewBookInfo() {
    delete ui;
}

void ViewBookInfo::backButtonClicked() {
    emit goBack();
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
    transactionManager.setBookAvailibityOptions(this, book, username);
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
