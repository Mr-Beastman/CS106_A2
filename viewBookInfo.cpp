#include "viewBookInfo.h"
#include "managementBook.h"
#include "ui_viewBookInfo.h"
#include "managementUser.h"



ViewBookInfo::ViewBookInfo(QWidget* parent) : QDialog(parent), ui(new Ui::ViewBookInfo) {
    ui->setupUi(this);

    //connect buttons
    connect(ui->backButton, &QPushButton::clicked, this, &ViewBookInfo::backButtonClicked);
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
