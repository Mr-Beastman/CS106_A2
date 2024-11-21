#include "memberView.h"
#include "mainWindow.h"
#include "updateUserView.h"

MemberView::MemberView(QWidget *parent) : QWidget(parent), ui(new Ui::MemberView) {
    ui->setupUi(this);

    MainWindow* mainWindow = dynamic_cast<MainWindow*>(parent);
    UserManagement* userManager = mainWindow->getUserManager();

    displayCurrentMember(userManager->getCurrentUser());

    //connect buttons
    connect(ui->updateButton, &QPushButton::clicked, this, &MemberView::updateButtonClicked);
}

MemberView::~MemberView() {
    delete ui;
}

//view related functions
void MemberView::displayCurrentMember(const QJsonObject& currentUser) {

    if(!currentUser.isEmpty()){
        ui->nameOutputLabel->setText(currentUser["name"].toString());
        ui->accountOutputLabel->setText(QString::number(currentUser["account"].toInt()));
        ui->phoneOutputLabel->setText(currentUser["phone"].toString());
        ui->emailOutputLabel->setText(currentUser["email"].toString());
        ui->addressOutputLabel->setText(currentUser["address"].toString());
    } else {
        qDebug()<<"User Not found";
    }
}

void MemberView::refreashView(){
    //accessing to MainWindow to force refreash
    MainWindow* mainWindow = nullptr;
    QWidget* parentWidget = qobject_cast<QWidget*>(parent());

    while(parentWidget != nullptr){
        mainWindow = dynamic_cast<MainWindow*>(parentWidget);
        if(mainWindow != nullptr){
            break;
        }
        parentWidget = parentWidget->parentWidget();
    }

    if(mainWindow != nullptr){
        qDebug()<<"Refreashing Member View";
        UserManagement* userManager = mainWindow->getUserManager();
        displayCurrentMember(userManager->getCurrentUser());
    } else {
        qDebug()<<"Failed to refreash Window";
    }
}



//accountTab functions
void MemberView::updateButtonClicked() {
    UpdateUserView* updateUserView = new UpdateUserView(this);
    connect(updateUserView, &UpdateUserView::requestRefreash, this, &MemberView::refreashView);
    updateUserView->exec();
}

