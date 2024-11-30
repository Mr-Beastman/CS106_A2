#include "memberInfoView.h"
#include "bookManagement.h"
#include "ui_memberInfoView.h"
#include "userManagement.h"
#include "updateUserView.h"

MemberInfoView::MemberInfoView(QWidget* parent) : QDialog(parent), ui(new Ui::MemberInfoView) {
    ui->setupUi(this);

    connect(ui->backButton, &QPushButton::clicked, this, &MemberInfoView::backButtonClicked);
    connect(ui->activateButton, &QPushButton::clicked, this, &MemberInfoView::activateButtonClicked);
    connect(ui->updateButton, &QPushButton::clicked, this, &MemberInfoView::updateButtonClicked);
    connect(ui->deleteMember, &QPushButton::clicked, this, &MemberInfoView::deleteButtonClicked);
}

MemberInfoView::~MemberInfoView(){
    delete ui;
}

void MemberInfoView::updateDisplay(const QJsonObject& updatedMember){
    setMemberDetails(updatedMember);
}

void MemberInfoView::backButtonClicked(){
    emit goBack();
}

void MemberInfoView::activateButtonClicked(){
    UserManagement* userManager = UserManagement::getUserManager();
    userManager->activateUser(ui->accountOutputLabel->text());
    updateDisplay(userManager->getUserObjAccount(ui->accountOutputLabel->text()));
    emit requestUpdateDisplay();
}

void MemberInfoView::deleteButtonClicked(){
    UserManagement* userManager = UserManagement::getUserManager();

    userManager->deleteMember(ui->accountOutputLabel->text());

    emit requestUpdateDisplay();
    emit goBack();
}

void MemberInfoView::updateButtonClicked(){
    UpdateUserView* updateUserView = new UpdateUserView(this);
    updateUserView->setAccountNumber(ui->accountOutputLabel->text());
    updateUserView->preloadUser(ui->accountOutputLabel->text());
    updateUserView->exec();

    UserManagement* userManager = UserManagement::getUserManager();

    QJsonObject user = userManager->getUserObjAccount(ui->accountOutputLabel->text());
    updateDisplay(user);
    emit requestUpdateDisplay();
}

bool MemberInfoView::setMemberDetails(const QJsonObject &userToView){

    if(userToView.isEmpty()){
        qDebug()<<"MemberInfoView: User details are empty";
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

    qDebug()<<"MemberInfoView: User detials have been populated";
    return true;
}
