#include "adminView.h"
#include "ui_adminView.h"
#include "addBookview.h"
#include "userManagement.h"

AdminView::AdminView(QWidget *parent) : QWidget(parent) {
    ui.setupUi(this);

    //set up for manage catalogue
    connect(ui.addButton, &QPushButton::clicked, this, &AdminView::addButtonClicked);
}

void AdminView::displayUsers() {

    UserManagement *userManager = UserManagement::getUserManager();

    QJsonArray data = userManager->getFileData()["users"].toArray();

    for(int i = 0; i<data.size(); ++i){
        QJsonObject user = data[i].toObject();

        //creating widget to hold user detials
        QWidget *userWidget = new QWidget();
        QHBoxLayout *layout = new QHBoxLayout(userWidget);

        //adding the users details
        QLabel*nameEntryLabel = new QLabel("Name: " +user["name"].toString());
        QLabel*accountEntryLabel = new QLabel("account #: " +user["account"].toString());
        QLabel*phoneEntryLabel = new QLabel("Phone: " +user["phone"].toString());
        QLabel*emailEntryLabel = new QLabel("Email: " +user["email"].toString());

        //adding widgest to layout
        layout->addWidget(nameEntryLabel);
        layout->addWidget(accountEntryLabel);
        layout->addWidget(phoneEntryLabel);
        layout->addWidget(emailEntryLabel);


        QListWidgetItem *entry = new QListWidgetItem();
        entry->setSizeHint(userWidget->sizeHint());

        //adding alternating colors
        if(i % 2 == 0){
            entry->setBackground(QBrush(QColor(158,206,104)));
        } else {
            entry->setBackground(QBrush(QColor(187,211,180)));
        }

        //adding entry widget to the ui file
        ui.membersList->addItem(entry);
        ui.membersList->setItemWidget(entry, userWidget);
    }

    qDebug()<<"User Entries created";
}

//manage catalogue functions
void AdminView::addButtonClicked(){
    AddBookView addBookDialog(this);
    addBookDialog.exec();
}
