#include "memberView.h"
#include "updateUserView.h"
#include "bookManagement.h"
#include "userManagement.h"
#include "bookListView.h"

MemberView::MemberView(QWidget *parent) : QWidget(parent), ui(new Ui::MemberView) {
    ui->setupUi(this);

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
    UserManagement* userManager = UserManagement::getUserManager();
    displayCurrentMember(userManager->getCurrentUser());
}



//accountTab functions
void MemberView::updateButtonClicked() {
    UpdateUserView* updateUserView = new UpdateUserView(this);
    connect(updateUserView, &UpdateUserView::requestRefreash, this, &MemberView::refreashView);
    updateUserView->exec();
}

//catalogueTab functions
void MemberView::loadCatalogue(){

    BookManagement *bookManager = BookManagement::getBookManager();

    QJsonObject database = bookManager->getFileData();

    QJsonArray bookData = database["books"].toArray();

    //clear list
    ui->catalogueList->clear();

    //looping throigh books to create a entry for it
    for(const QJsonValue& bookValue : bookData){
        QJsonObject book = bookValue.toObject();

        BookListView* bookListView = new BookListView(ui->catalogueList);

        // Adding data to the entry
        QLabel* titleLabel = bookListView->findChild<QLabel*>("titleOutputLabel");
        if (titleLabel) {
            titleLabel->setText(book["title"].toString());
        } else {
            qDebug() << "Error: title label not found!";
        }

        QLabel* authorLabel = bookListView->findChild<QLabel*>("authorOutputLabel");
        if (authorLabel) {
            authorLabel->setText(book["author"].toString());
        } else {
            qDebug() << "Error: author label not found!";
        }

        QLabel* isbnLabel = bookListView->findChild<QLabel*>("isbnOutputLabel");
        if (isbnLabel) {
            isbnLabel->setText(book["isbn"].toString());
        } else {
            qDebug() << "Error: ISBN label not found!";
        }

        // adding cover image
        QLabel* coverLabel = bookListView->findChild<QLabel*>("coverLabel");
        if (coverLabel) {
            QString coverImagePath = bookManager->findCoverPath() + book["isbn"].toString() + ".png";
            QPixmap cover(coverImagePath);
            QString noCoverImagePath = bookManager->findCoverPath() + "noCover.png";
            QPixmap noCover(noCoverImagePath);

            if (!cover.isNull()) {
                coverLabel->setPixmap(cover);
            } else {
                coverLabel->setPixmap(noCover);
            }
            coverLabel->setScaledContents(true);
        } else {
            qDebug() << "Error: cover label not found!";
        }

        //adding entry to list
        QListWidgetItem* item = new QListWidgetItem(ui->catalogueList);
        item->setSizeHint(bookListView->sizeHint());
        ui->catalogueList->setItemWidget(item, bookListView);
    }
}
