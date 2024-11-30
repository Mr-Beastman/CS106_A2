#include "updateUserView.h"
#include "ui_updateUserView.h"
#include "mainWindow.h"

UpdateUserView::UpdateUserView(QWidget *parent) : QDialog(parent), ui(new Ui::UpdateUserView) {
    ui->setupUi(this);

    // set up title
    this->setWindowTitle("Update Details");
    this->setModal(true);

    connect(ui->saveButton, &QPushButton::clicked, this, &UpdateUserView::saveButtonClicked);
}

UpdateUserView::~UpdateUserView() {
    delete ui;
}

void UpdateUserView::setAccountNumber(const QString& account){
    accountNumber = account;
}

QString UpdateUserView::getAccountNumber(){
    return accountNumber;
}

void UpdateUserView::preloadUser(const QString& account) {

    UserManagement* userManager = UserManagement::getUserManager();

    QJsonObject user = userManager->getUserObjAccount(account);

    //preload lineEdits
    ui->nameLineEdit->setText(user["name"].toString());
    ui->phoneLineEdit->setText(user["phone"].toString());
    ui->emailLineEdit->setText(user["email"].toString());
    ui->addressLineEdit->setText(user["address"].toString());

}

void UpdateUserView::saveButtonClicked() {

    UserManagement* userManager = UserManagement::getUserManager();

    QJsonObject updateDetails;

    updateDetails["name"] = ui->nameLineEdit->text();
    updateDetails["phone"] = ui->phoneLineEdit->text();
    updateDetails["email"] = ui->emailLineEdit->text();
    updateDetails["address"] = ui->addressLineEdit->text();


    if(userManager->updateUser(accountNumber,updateDetails)){
        accept();
    } else {
        qDebug()<<"UpdateUserView: Displaying Error Message";
        ui->errorLabel->setText("No updates detected");
    }
}
