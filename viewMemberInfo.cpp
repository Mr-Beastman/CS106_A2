#include "viewMemberInfo.h"
#include "managementBook.h"
#include "ui_viewMemberInfo.h"
#include "managementUser.h"
#include "viewUpdateUser.h"

ViewMemberInfo::ViewMemberInfo(QWidget* parent, const QJsonObject &user) : QDialog(parent), ui(new Ui::ViewMemberInfo) {
    ui->setupUi(this);

    connect(ui->backButton, &QPushButton::clicked, this, &ViewMemberInfo::backButtonClicked);
    connect(ui->activateButton, &QPushButton::clicked, this, &ViewMemberInfo::activateButtonClicked);
    connect(ui->updateButton, &QPushButton::clicked, this, &ViewMemberInfo::updateButtonClicked);
    connect(ui->deleteMember, &QPushButton::clicked, this, &ViewMemberInfo::deleteButtonClicked);

    // generateCheckedout(user["account"].toString());
    // displayHoldRequests(user["account"].toString());
}

ViewMemberInfo::~ViewMemberInfo(){
    delete ui;
}

void ViewMemberInfo::updateDisplay(const QJsonObject& updatedMember){
    setMemberDetails(updatedMember);
}

void ViewMemberInfo::generateCheckedout(const QString account){
    qDebug()<<"viewMemberInfo: Checking for active loans";

    ManagementUser userManager;
    ManagementBook bookManager;

    QJsonObject user = userManager.getUserObjAccount(account);
    QJsonArray activeLoans = user["activeLoans"].toArray();

    //if no checked out books, set display to reflect this
    if(activeLoans.isEmpty()){
        qDebug()<<"viewMemberInfo: No Active loans found. Setting to empty";
        ui->issuedStackedWidget->setCurrentWidget(ui->noIssuedPage);
        return;
    }

    qDebug()<<"viewMemberInfo: Active loans found, generating list";
    ui->issuedStackedWidget->setCurrentWidget(ui->issuedPage);

    //ensure clean list on updating
    ui->issuedList->clear();

    for(int i = 0; i<activeLoans.size(); i++){
        QJsonObject loan = activeLoans[i].toObject();
        QJsonObject book = bookManager.getBookDetails(loan["isbn"].toString());

        ViewBookItem* listItem = bookManager.createBookList(book, loan);

        //setting display
        QStackedWidget* stackedWidget = listItem->findChild<QStackedWidget*>("optionsStackedWidget");
        int index = stackedWidget->indexOf(listItem->findChild<QWidget*>("userCheckedoutPage"));
        stackedWidget->setCurrentIndex(index);
        QLabel* dueLabel = listItem->findChild<QLabel*>("dueOutputLabel");
        dueLabel->setText(loan["dueDate"].toString());

        //adding loan entry to list
        QListWidgetItem* entry = new QListWidgetItem(ui->issuedList);

        //assigning isbn to item
        entry->setData(Qt::UserRole, book["isbn"].toString());

        //entry display settings
        entry->setSizeHint(listItem->sizeHint());

        if(i % 2 == 0){
            entry->setBackground(QBrush(QColor(158,206,104)));
        } else {
            entry->setBackground(QBrush(QColor(187,211,180)));
        }

        ui->issuedList->setItemWidget(entry, listItem);
    }

}

void ViewMemberInfo::displayHoldRequests(const QString accountNumber){
    qDebug()<<"viewMemberInfo: Checking for active hold requests";

    //set managers
    ManagementUser userManager;
    ManagementBook bookManager;

    //get user and book data
    QJsonObject user = userManager.getUserObjAccount(accountNumber);
    QJsonArray activeHolds = user["holdRequests"].toArray();
    QJsonArray bookArray = bookManager.getBookArray();

    //end if user has no active holds
    if(activeHolds.isEmpty()){
        qDebug()<<"ViewMemberInfo: No hold requets to display";
        ui->holdStackedWidget->setCurrentWidget(ui->noHoldPage);
        return;
    }

    qDebug()<<"viewMemberInfo: Active holds found, generating list";
    ui->holdStackedWidget->setCurrentWidget(ui->holdPage);

    //ensure clean list on updating
    ui->holdList->clear();

    for(int i = 0; i<activeHolds.size(); i++){
        QJsonObject hold = activeHolds[i].toObject();
        QString holdIsbn  = hold["isbn"].toString();
        QString holdId = hold["holdId"].toString();

        for(int j = 0; j<bookArray.size(); j++){
            QJsonObject book = bookArray[j].toObject();

            if(book["isbn"].toString() == holdIsbn){

                QJsonArray bookQueue = book["inQueue"].toArray();

                for(int k = 0; k<bookQueue.size(); k++){
                    QJsonObject userPlace = bookQueue[k].toObject();

                    if(userPlace["holdId"] == holdId){
                        QString holdStatus = userPlace["holdStatus"].toString();

                        ViewBookItem* listItem = bookManager.createBookList(book, hold);

                        //setting display
                        QStackedWidget* availibiltyWidget = listItem->findChild<QStackedWidget*>("availabilityWidget");
                        QStackedWidget* optionsWidget = listItem->findChild<QStackedWidget*>("optionsStackedWidget");
                        int index;

                        if(holdStatus == "active"){
                            index = availibiltyWidget->indexOf(listItem->findChild<QWidget*>("holdPendingPage"));
                            availibiltyWidget->setCurrentIndex(index);
                            index = optionsWidget->indexOf(listItem->findChild<QWidget*>("holdActiveLabelLayout"));
                            optionsWidget->setCurrentIndex(index);
                            QLabel* placeInQueue = listItem->findChild<QLabel*>("queuePlaceOutPutLabel");
                            placeInQueue->setText(QString::number(k+1));

                        } else {
                            index = availibiltyWidget->indexOf(listItem->findChild<QWidget*>("holdReadyPage"));
                            availibiltyWidget->setCurrentIndex(index);
                            optionsWidget->hide();
                        }

                        QLabel* holdIdWidget = listItem->findChild<QLabel*>("holdIdOutputLabel");
                        holdIdWidget->setText(holdId);

                        //adding loan entry to list
                        QListWidgetItem* entry = new QListWidgetItem(ui->holdList);

                        //assigning isbn to item
                        entry->setData(Qt::UserRole, book["isbn"].toString());

                        //entry display settings
                        entry->setSizeHint(listItem->sizeHint());

                        if(i % 2 == 0){
                            entry->setBackground(QBrush(QColor(158,206,104)));
                        } else {
                            entry->setBackground(QBrush(QColor(187,211,180)));
                        }

                        ui->holdList->setItemWidget(entry, listItem);
                    }
                }
            }
        }
    }
}

void ViewMemberInfo::backButtonClicked(){
    emit goBackAdmin();
}

void ViewMemberInfo::activateButtonClicked(){
    ManagementUser userManager;
    userManager.activateUser(ui->accountOutputLabel->text());
    updateDisplay(userManager.getUserObjAccount(ui->accountOutputLabel->text()));
    emit requestUpdateDisplay();
}

void ViewMemberInfo::deleteButtonClicked(){
    ManagementUser userManager;

    userManager.deleteMember(ui->accountOutputLabel->text());

    emit requestUpdateDisplay();
    emit goBackAdmin();
}

void ViewMemberInfo::updateButtonClicked(){
    ViewUpdateUser* viewUpdateUser = new ViewUpdateUser(this);
    viewUpdateUser->setAccountNumber(ui->accountOutputLabel->text());
    viewUpdateUser->preloadUser(ui->accountOutputLabel->text());
    viewUpdateUser->exec();

    ManagementUser userManager;

    QJsonObject user = userManager.getUserObjAccount(ui->accountOutputLabel->text());
    updateDisplay(user);
    emit requestUpdateDisplay();
}

bool ViewMemberInfo::setMemberDetails(const QJsonObject &userToView){

    if(userToView.isEmpty()){
        qDebug()<<"viewMemberInfo: User details are empty";
        return false;
    }

    ui->nameOutputLabel->setText(userToView["name"].toString());
    ui->accountOutputLabel->setText(userToView["account"].toString());
    ui->phoneOutputLabel->setText(userToView["phone"].toString());
    ui->emailOutputLabel->setText(userToView["email"].toString());
    ui->addressOutputLabel->setText(userToView["address"].toString());

    if(userToView["isActive"].toBool()){
        ui->activeStackedWidget->setCurrentIndex(ui->activeStackedWidget->indexOf(ui->activePage));
    } else {
        ui->activeStackedWidget->setCurrentIndex(ui->activeStackedWidget->indexOf(ui->activatePage));
    }

    qDebug()<<"viewMemberInfo: User detials have been populated";
    return true;
}
