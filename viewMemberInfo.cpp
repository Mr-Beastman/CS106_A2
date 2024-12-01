#include "viewMemberInfo.h"
#include "managementBook.h"
#include "ui_viewMemberInfo.h"
#include "managementUser.h"
#include "viewUpdateUser.h"

ViewMemberInfo::ViewMemberInfo(QWidget* parent) : QDialog(parent), ui(new Ui::ViewMemberInfo) {
    ui->setupUi(this);

    connect(ui->backButton, &QPushButton::clicked, this, &ViewMemberInfo::backButtonClicked);
    connect(ui->activateButton, &QPushButton::clicked, this, &ViewMemberInfo::activateButtonClicked);
    connect(ui->updateButton, &QPushButton::clicked, this, &ViewMemberInfo::updateButtonClicked);
    connect(ui->deleteMember, &QPushButton::clicked, this, &ViewMemberInfo::deleteButtonClicked);
}

ViewMemberInfo::~ViewMemberInfo(){
    delete ui;
}

void ViewMemberInfo::updateDisplay(const QJsonObject& updatedMember){
    setMemberDetails(updatedMember);
}

void ViewMemberInfo::generateCheckedout(const QString account){
    qDebug()<<"viewMemberInfo: Checking for active loans";

    managementUser userManager;
    managementBook bookManager;



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

void ViewMemberInfo::backButtonClicked(){
    emit goBack();
}

void ViewMemberInfo::activateButtonClicked(){
    managementUser userManager;
    userManager.activateUser(ui->accountOutputLabel->text());
    updateDisplay(userManager.getUserObjAccount(ui->accountOutputLabel->text()));
    emit requestUpdateDisplay();
}

void ViewMemberInfo::deleteButtonClicked(){
    managementUser userManager;

    userManager.deleteMember(ui->accountOutputLabel->text());

    emit requestUpdateDisplay();
    emit goBack();
}

void ViewMemberInfo::updateButtonClicked(){
    ViewUpdateUser* viewUpdateUser = new ViewUpdateUser(this);
    viewUpdateUser->setAccountNumber(ui->accountOutputLabel->text());
    viewUpdateUser->preloadUser(ui->accountOutputLabel->text());
    viewUpdateUser->exec();

    managementUser userManager;

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
