#include "viewMemberInfo.h"
#include "managementBook.h"
#include "managementTransaction.h"
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
    ManagementTransaction transactionManager;

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

        //set book availbity and the options aavialble
        transactionManager.setBookAvailibityOptions(listItem, book, user["username"].toString());

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
    ManagementTransaction transactionManager;

    //get user and book data
    QJsonObject user = userManager.getUserObjAccount(accountNumber);
    QJsonArray activeHolds = user["holdRequests"].toArray();
    QJsonArray bookArray = bookManager.getBookArray();
    QJsonArray holdsArray = transactionManager.getHoldArray();

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

    qDebug()<<"ViewMemberDashboard: Generating users hold items";
    //build items for list to display
    for(int i = 0; i<activeHolds.size(); ++i){
        QJsonObject userHold = activeHolds[i].toObject();

        for(int j = 0; j<holdsArray.size(); j++){
            QJsonObject storedHold = holdsArray[i].toObject();

            if(userHold["holdId"].toString() == storedHold["holdId"].toString()){

                QJsonObject book = bookManager.getBookDetails(storedHold["isbn"].toString());

                qDebug()<<book["title"].toString();

                //creating viewBookItem for hold
                ViewBookItem* viewBookItem = bookManager.createBookList(book, userHold);

                //adding entry to list
                QListWidgetItem* item = new QListWidgetItem(ui->holdList);

                //adding alternating colors
                if(i % 2 == 0){
                    item->setBackground(QBrush(QColor(158,206,104)));
                } else {
                    item->setBackground(QBrush(QColor(187,211,180)));
                }

                item->setSizeHint(viewBookItem->sizeHint());

                //storing hidden values for transactions
                QString holdId = userHold["holdId"].toString();
                QLabel* idLabel = viewBookItem->findChild<QLabel*>("holdStoredIdLabel");
                idLabel->setText(holdId);
                QLabel* usernameLabel = viewBookItem->findChild<QLabel*>("usernameStoredLabel");
                usernameLabel->setText(user["username"].toString());

                //setting availability and members action displays
                //set book availbity and the options aavialble
                transactionManager.setBookAvailibityOptions(viewBookItem, book, user["username"].toString());
                ui->holdList->setItemWidget(item, viewBookItem);
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
