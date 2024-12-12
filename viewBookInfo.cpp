#include "viewBookInfo.h"
#include "managementBook.h"
#include "managementTransaction.h"
#include "ui_viewBookInfo.h"




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

void ViewBookInfo::setBookAvailibity(const QJsonObject& book, const QString& username){

    qDebug()<<"ViewBookInfo: Setting Availbility and Options";
    qDebug()<<book["title"].toString();
    qDebug()<<username;
    QStackedWidget* availabilityWidget = this->findChild<QStackedWidget*>("availabilityWidget");
    QStackedWidget* optionsWidget = this->findChild<QStackedWidget*>("optionsStackedWidget");
    QWidget* availabilityPage = nullptr;
    QWidget* optionsPage = nullptr;
    int index = 0;

    if (book["isAvailable"].toBool()) {
        //book is available
        availabilityPage = this->findChild<QWidget*>("availablePage");
        optionsPage = this->findChild<QWidget*>("userAvailablePage");
    } else if (book["issuedTo"].toString() == username) {
        //book is checked out by the current user
        qDebug()<<"ViewBookInfo: Book is issued to currrent user";
        availabilityPage = this->findChild<QWidget*>("checkedoutPage");
        optionsPage = this->findChild<QWidget*>("userCheckedoutPage");
    } else {
        //book is unavailable, checking if the user has a hold
        bool userHold = false;
        QString status;
        QString holdId;

        ManagementTransaction transactionManager;
        QJsonArray holdsArray = transactionManager.getHoldArray();

        for (int j = 0; j < holdsArray.size(); ++j) {
            QJsonObject hold = holdsArray[j].toObject();
            if (hold["isbn"].toString() == book["isbn"].toString() && hold["username"].toString() == username) {
                userHold = true;
                holdId = hold["holdId"].toString();
                status = hold["holdStatus"].toString();
                break;
            }
        }

        if (userHold) {
            //storing hold ID
            QLabel* holdIdLabel = this->findChild<QLabel*>("holdStoredIdLabel");
            holdIdLabel->setText(holdId);

            if (status == "ready") {
                availabilityPage = this->findChild<QWidget*>("holdReadyDisplayPage");
                optionsPage = this->findChild<QWidget*>("holdReadyPage");
            } else {
                availabilityPage = this->findChild<QWidget*>("holdPendingPage");
                optionsPage = this->findChild<QWidget*>("holdActivePage");
            }
        } else {
            //allow user to request hold
            availabilityPage = this->findChild<QWidget*>("notAvailablePage");
            optionsPage = this->findChild<QWidget*>("userNotAvailablePage");
        }
    }

    if (availabilityPage) {
        qDebug()<<"ViewBookInfo: Setting avialbilty page";
        index = availabilityWidget->indexOf(availabilityPage);
        availabilityWidget->setCurrentIndex(index);
    }
    if (optionsPage) {
        qDebug()<<"ViewBookInfo: Setting options page";
        index = optionsWidget->indexOf(optionsPage);
        optionsWidget->setCurrentIndex(index);
    }
}
