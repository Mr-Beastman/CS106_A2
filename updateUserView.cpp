#include "updateUserView.h"
#include "mainWindow.h"

UpdateUserView::UpdateUserView(QWidget *parent) : QDialog(parent), ui(new Ui::UpdateUserView) {
    ui->setupUi(this);

    // set up title
    this->setWindowTitle("Update Details");
    this->setModal(true);

    preloadUser();

    connect(ui->saveButton, &QPushButton::clicked, this, &UpdateUserView::saveButtonClicked);
}

UpdateUserView::~UpdateUserView() {
    delete ui;
}

void UpdateUserView::preloadUser() {

    //logic to handle different parents eg admin or member view
    QWidget* childOf = this->parentWidget();
    MainWindow* mainWindow=nullptr;

    while(childOf){
        mainWindow=dynamic_cast<MainWindow*>(childOf);
        if(mainWindow){
            break;
        }
        childOf = childOf->parentWidget();
    }

    UserManagement* userManager = mainWindow->getUserManager();

    QJsonObject currentUser = userManager->getCurrentUser();

    //preload lineEdits
    ui->nameLineEdit->setText(currentUser["name"].toString());
    ui->phoneLineEdit->setText(currentUser["phone"].toString());
    ui->emailLineEdit->setText(currentUser["email"].toString());
    ui->addressLineEdit->setText(currentUser["address"].toString());

}

void UpdateUserView::saveButtonClicked() {
    //logic to handle different parents eg admin or member view
    QWidget* childOf = this->parentWidget();
    MainWindow* mainWindow=nullptr;

    while(childOf){
        mainWindow=dynamic_cast<MainWindow*>(childOf);
        if(mainWindow){
            break;
        }
        childOf = childOf->parentWidget();
    }

    UserManagement* userManager = mainWindow->getUserManager();
    QJsonObject currentUser = userManager->getCurrentUser();

    //store values from from ui lineEdits
    QString updatedName = ui->nameLineEdit->text();
    QString updatedPhone = ui->phoneLineEdit->text();
    QString updatedEmail = ui->emailLineEdit->text();
    QString updatedAddress = ui->addressLineEdit->text();

    //check if any chages have been made
    bool updated = false;

    if(currentUser["name"].toString() != updatedName){
        currentUser["name"] = updatedName;
        updated = true;
    }
    if(currentUser["phone"].toString() != updatedPhone){
        currentUser["phone"] = updatedPhone;
        updated = true;
    }
    if(currentUser["email"].toString() != updatedEmail){
        currentUser["email"] = updatedEmail;
        updated = true;
    }
    if(currentUser["address"].toString() != updatedAddress){
        currentUser["address"] = updatedAddress;
        updated = true;
    }

    //if updated loop through current userdata to update values
    if(updated){
        QJsonArray usersArray = userManager->getFileData()["users"].toArray();
        for(int i = 0; usersArray.size(); ++i){
            QJsonObject user = usersArray[i].toObject();
            if(user["username"].toString() == currentUser["username"].toString()){
                usersArray[i] = currentUser;
                break;
            }
        }

        QJsonObject fileData = userManager -> getFileData();
        fileData["users"] = usersArray;
        userManager->setFileData(fileData);

        if(userManager->saveData()){
            qDebug()<<"UpdateUserView: User has been updated";
            //update the store value
            userManager->setCurrentUser(currentUser["username"].toString());
            //signal memberView to refreash
            emit requestRefreash();
        } else {
            qDebug()<<"UpdateUserView: could not be updated";
        }

        //close the window on save

        this->accept();

    } else {
        qDebug()<<"UpdateUserView: No changes detected";
        ui->errorLabel->setText("No Changes to Save");
    }


}
