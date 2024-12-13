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
    QStackedWidget* availabilityWidget = this->findChild<QStackedWidget*>("availabilityWidget");
    QStackedWidget* optionsWidget = this->findChild<QStackedWidget*>("optionsStackedWidget");
    QWidget* availabilityPage = nullptr;
    QWidget* optionsPage = nullptr;
    int index = 0;

    if(transactionManager.isAdmin(username)){
        //setting admin displays
        index = optionsWidget->indexOf(this->findChild<QWidget*>("adminPage"));
        optionsWidget->setCurrentIndex(index);

        //getting current book activity.
        QLabel* activeLoan  =  this->findChild<QLabel*>("checkedOutputLabel");
        activeLoan->setText(transactionManager.checkedOutTo(book["isbn"].toString()));

        QJsonArray queue = book["inQueue"].toArray();

        QLabel* holdArray =  this->findChild<QLabel*>("QueueOutputLabel");
        if(queue.isEmpty()){
            holdArray->setText(QString::number(book["inQueue"].toArray().size()));
        } else {
            holdArray->setText(QString::number(0));
        }

        //enabling/disabling return button depending on status
        QPushButton* returnButton = this->findChild<QPushButton*>("returnButton");
        availabilityPage = this->findChild<QStackedWidget*>("availabilityWidget");

        if(book["isAvailable"].toBool()){
            returnButton->hide();
            availabilityPage = this->findChild<QWidget*>("availablePage");
        } else {
            if(!queue.isEmpty()){
                QJsonObject firstInQueue = queue[0].toObject();
                QString holdStatus = firstInQueue["holdStatus"].toString();

                if(holdStatus == "ready"){
                    returnButton->hide();
                    availabilityPage = this->findChild<QWidget*>("holdReadyPage");
                } else if (holdStatus == "active"){
                    returnButton->show();
                    availabilityPage = this->findChild<QWidget*>("notAvailablePage");
                }
            } else {
                returnButton->show();
                availabilityPage = this->findChild<QWidget*>("notAvailablePage");
            }
        }

        QLabel* queueLabel = this->findChild<QLabel*>("QueueOutputLabel");
        queueLabel->setText(QString::number(queue.size()));
    } else {
        //setting members displays
        if (book["isAvailable"].toBool()) {
            //book is available
            availabilityPage = this->findChild<QWidget*>("availablePage");
            optionsPage = this->findChild<QWidget*>("userAvailablePage");
        } else if (book["issuedTo"].toString() == username) {
            //book is checked out by the current user
            if(transactionManager.bookIsDue(username, book["isbn"].toString())){
                availabilityPage = this->findChild<QWidget*>("dueBackPage");
            } else if(transactionManager.bookIsOverDue(username, book["isbn"].toString())) {
                availabilityPage = this->findChild<QWidget*>("overduePage");
            } else {
                availabilityPage = this->findChild<QWidget*>("checkedoutPage");
            }
            optionsPage = this->findChild<QWidget*>("userCheckedoutPage");
            //setting due date
            QLabel* dueLabel = this->findChild<QLabel*>("dueOutputLabel");
            dueLabel->setText(transactionManager.getDueDate(username, book["isbn"].toString()));
        } else {
            //book is unavailable, checking if the user has a hold
            bool userHold = false;
            QString status;
            QString holdId;
            int place;


            QJsonArray holdArray = transactionManager.getHoldArray();
            for (int j = 0; j < holdArray.size(); ++j) {
                QJsonObject hold = holdArray[j].toObject();
                if (hold["isbn"].toString() == book["isbn"].toString() && hold["username"].toString() == username) {
                    userHold = true;
                    holdId = hold["holdId"].toString();
                    status = hold["holdStatus"].toString();
                    break;
                }
            }

            if (userHold) {
                //getting place number
                QJsonArray holds = book["inQueue"].toArray();
                for(int k = 0; k<holds.size(); k++){
                    QJsonObject hold = holds[k].toObject();

                    if(hold["holdId"].toString()== holdId){
                        place = k+1;
                        break;
                    }
                }

                //storing hold ID
                QLabel* holdIdLabel = this->findChild<QLabel*>("holdStoredIdLabel");
                holdIdLabel->setText(holdId);

                if (status == "ready") {
                    availabilityPage = this->findChild<QWidget*>("holdReadyDisplayPage");
                    optionsPage = this->findChild<QWidget*>("holdReadyPage");
                } else {
                    availabilityPage = this->findChild<QWidget*>("holdPendingPage");
                    optionsPage = this->findChild<QWidget*>("holdActivePage");
                    QLabel* dueLabel = this->findChild<QLabel*>("queuePlaceOutPutLabel");
                    dueLabel->setNum(place);
                }
            } else {
                //allow user to request hold
                availabilityPage = this->findChild<QWidget*>("notAvailablePage");
                optionsPage = this->findChild<QWidget*>("userNotAvailablePage");
            }
        }
    }

    if (availabilityPage) {
        index = availabilityWidget->indexOf(availabilityPage);
        availabilityWidget->setCurrentIndex(index);
    }
    if (optionsPage) {
        index = optionsWidget->indexOf(optionsPage);
        optionsWidget->setCurrentIndex(index);
    }
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
