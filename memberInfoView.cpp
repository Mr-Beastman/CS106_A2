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

void MemberInfoView::generateCheckedout(const QString account){
    qDebug()<<"MemberInfoView: Checking for active loans";

    UserManagement userManager;
    BookManagement bookManager;



    QJsonObject user = userManager.getUserObjAccount(account);
    QJsonArray activeLoans = user["activeLoans"].toArray();

    //if no checked out books, set display to reflect this
    if(activeLoans.isEmpty()){
        qDebug()<<"MemberInfoView: No Active loans found. Setting to empty";
        ui->issuedStackedWidget->setCurrentWidget(ui->noIssuedPage);
        return;
    }

    qDebug()<<"MemberInfoView: Active loans found, generating list";
    ui->issuedStackedWidget->setCurrentWidget(ui->issuedPage);

    ui->issuedList->clear();

    for(int i = 0; i<activeLoans.size(); i++){
        QJsonObject loan = activeLoans[i].toObject();
        QJsonObject book = bookManager.getBookDetails(loan["isbn"].toString());

        BookListView* listItem = bookManager.createBookList(book, loan);

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

void MemberInfoView::backButtonClicked(){
    emit goBack();
}

void MemberInfoView::activateButtonClicked(){
    UserManagement userManager;
    userManager.activateUser(ui->accountOutputLabel->text());
    updateDisplay(userManager.getUserObjAccount(ui->accountOutputLabel->text()));
    emit requestUpdateDisplay();
}

void MemberInfoView::deleteButtonClicked(){
    UserManagement userManager;

    userManager.deleteMember(ui->accountOutputLabel->text());

    emit requestUpdateDisplay();
    emit goBack();
}

void MemberInfoView::updateButtonClicked(){
    UpdateUserView* updateUserView = new UpdateUserView(this);
    updateUserView->setAccountNumber(ui->accountOutputLabel->text());
    updateUserView->preloadUser(ui->accountOutputLabel->text());
    updateUserView->exec();

    UserManagement userManager;

    QJsonObject user = userManager.getUserObjAccount(ui->accountOutputLabel->text());
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
