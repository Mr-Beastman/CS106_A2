#include "addMemberView.h"
#include "userManagement.h"
#include "qjsonobject.h"
#include "ui_addMemberView.h"

AddMemberView::AddMemberView(QWidget *parent) : QDialog(parent), ui(new Ui::AddMemberView) {
    ui->setupUi(this);

    // set up title
    this->setWindowTitle("Add Member");
    this->setModal(true);

    connect(ui->saveButton, &QPushButton::clicked, this, &AddMemberView::addMemberButtonClicked);
}

AddMemberView::~AddMemberView() {
    delete ui;
}

void AddMemberView::addMemberButtonClicked(){
    QJsonObject newUser;
    UserManagement* userManager = UserManagement::getUserManager();

    newUser["name"]=ui->nameLineEdit->text();
    newUser["username"]=ui->usernameLineEdit->text();
    newUser["password"]=ui->passwordLineEdit->text();
    newUser["phone"]=ui->phoneLineEdit->text();
    newUser["email"]=ui->emailLineEdit->text();
    newUser["address"]=ui->nameLineEdit->text();

    if(!userManager->usernameExists(newUser)){
        userManager->addUser(newUser);
        accept();
    } else {
        qDebug()<<"AddMemberView: Setting username error message";
        ui->errorOutputLabel->setText("Username already taken. Please try again");
    }
}
