#include "memberView.h"
#include "qstackedwidget.h"
#include "updateUserView.h"
#include "bookManagement.h"
#include "userManagement.h"
#include "bookListView.h"

MemberView::MemberView(QWidget *parent) : QWidget(parent), ui(new Ui::MemberView) {
    ui->setupUi(this);

    //connect buttons
    connect(ui->updateButton, &QPushButton::clicked, this, &MemberView::updateButtonClicked);
    connect(ui->logoutButton, &QAbstractButton::clicked, this, &MemberView::logoutButtonClicked);
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
    qDebug()<<"MemberView: Refreashing member views to show updated information";
    UserManagement* userManager = UserManagement::getUserManager();
    BookManagement* bookManager = BookManagement::getBookManager();

    //ensure most current data is loaded
    bookManager->readData();
    bookManager->setBookArray();
    userManager->setUserArray();

    displayCurrentMember(userManager->getCurrentUser());
    displayCheckedOut(userManager->getCurrentUser());
    displayHoldRequests(userManager->getCurrentUser());
    loadCatalogue();
}

// void logoutButtonClicked(){
//     emit logoutRequest();
// }


//accountTab functions
void MemberView::updateButtonClicked() {
    UpdateUserView* updateUserView = new UpdateUserView(this);
    connect(updateUserView, &UpdateUserView::requestRefreash, this, &MemberView::refreashView);
    updateUserView->exec();
}

void MemberView::logoutButtonClicked() {
    emit logoutRequest();
}

//populates ui with books currently checked out to the current user
//parameters: QjsonObject containing user to display
//returns : none
void MemberView::displayCheckedOut(const QJsonObject& currentUser) {

    QJsonArray checkedOut = currentUser["activeLoans"].toArray();
    BookManagement* bookManager = BookManagement::getBookManager();

    //if array is empty end early
    if(checkedOut.isEmpty()){
        return;
    }

    ui->issuedList->clear();

    qDebug()<<"User has : "<<checkedOut.size()<<" books checked out";

    for(int i = 0; i<checkedOut.size(); ++i){
        QJsonObject entry = checkedOut[i].toObject();
        QJsonObject book = bookManager->getBookDetails(entry["isbn"].toString());

        BookListView* bookListView = new BookListView(ui->issuedList);
        connect(bookListView, &BookListView::refreashView, this, &MemberView::refreashView);

        // // Adding data to the entry
        QLabel* titleLabel = bookListView->findChild<QLabel*>("titleOutputLabel");
        titleLabel->setText(book["title"].toString());

        QLabel* authorLabel = bookListView->findChild<QLabel*>("authorOutputLabel");
        authorLabel->setText(book["author"].toString());

        QLabel* isbnLabel = bookListView->findChild<QLabel*>("isbnOutputLabel");
        isbnLabel->setText(book["isbn"].toString());

        QLabel* dueLabel = bookListView->findChild<QLabel*>("dueOutputLabel");
        dueLabel->setText(entry["dueDate"].toString());


        //display due date and renew options
        QStackedWidget* stackedWidget = bookListView->findChild<QStackedWidget*>("optionsStackedWidget");
        int index = stackedWidget->indexOf(bookListView->findChild<QWidget*>("userCheckedoutPage"));
        stackedWidget->setCurrentIndex(index);

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
            qDebug() << "MemberView: Cover label not found";
        }

        //adding entry to list
        QListWidgetItem* item = new QListWidgetItem(ui->issuedList);

        //adding alternating colors
        if(i % 2 == 0){
            item->setBackground(QBrush(QColor(158,206,104)));
        } else {
            item->setBackground(QBrush(QColor(187,211,180)));
        }

        connect(bookListView, &BookListView::refreashView, this, &MemberView::refreashView);

        item->setSizeHint(bookListView->sizeHint());
        ui->issuedList->setItemWidget(item, bookListView);
    }

    qDebug()<<"MemberView: Issued items for "<<currentUser["username"].toString()<<" displayed";
}

void MemberView::displayHoldRequests(const QJsonObject& currentUser) {

    QJsonArray holdRequests = currentUser["holdRequests"].toArray();
    BookManagement* bookManager = BookManagement::getBookManager();

    //if array is empty end early
    if(holdRequests.isEmpty()){
        return;
    }

    ui->holdList->clear();

    for(int i = 0; i<holdRequests.size(); ++i){
        QJsonObject entry = holdRequests[i].toObject();

        QJsonObject book = bookManager->getBookDetails(entry["isbn"].toString());

        BookListView* bookListView = new BookListView(ui->holdList);
        connect(bookListView, &BookListView::refreashView, this, &MemberView::refreashView);

        // // Adding data to the entry
        QLabel* titleLabel = bookListView->findChild<QLabel*>("titleOutputLabel");
        titleLabel->setText(book["title"].toString());

        QLabel* authorLabel = bookListView->findChild<QLabel*>("authorOutputLabel");
        authorLabel->setText(book["author"].toString());

        QLabel* isbnLabel = bookListView->findChild<QLabel*>("isbnOutputLabel");
        isbnLabel->setText(book["isbn"].toString());

        QLabel* dueLabel = bookListView->findChild<QLabel*>("dueOutputLabel");
        dueLabel->setText(entry["dueDate"].toString());


        //display due date and renew options
        QStackedWidget* stackedWidget = bookListView->findChild<QStackedWidget*>("optionsStackedWidget");
        int index = stackedWidget->indexOf(bookListView->findChild<QWidget*>("userCheckedoutPage"));
        stackedWidget->setCurrentIndex(index);

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
            qDebug() << "MemberView: Cover label not found";
        }

        //adding entry to list
        QListWidgetItem* item = new QListWidgetItem(ui->holdList);

        //adding alternating colors
        if(i % 2 == 0){
            item->setBackground(QBrush(QColor(158,206,104)));
        } else {
            item->setBackground(QBrush(QColor(187,211,180)));
        }

        connect(bookListView, &BookListView::refreashView, this, &MemberView::refreashView);

        item->setSizeHint(bookListView->sizeHint());
        ui->holdList->setItemWidget(item, bookListView);
    }

    qDebug()<<"MemberView: Issued items for "<<currentUser["username"].toString()<<" displayed";
}


//catalogueTab functions
void MemberView::loadCatalogue(){

    BookManagement *bookManager = BookManagement::getBookManager();


    QJsonArray& bookData = bookManager->getBookArray();

    //clear list
    ui->catalogueList->clear();

    //looping throigh books to create a entry for it
    for(int i = 0; i<bookData.size(); ++i){
        QJsonObject book = bookData[i].toObject();

        BookListView* bookListView = new BookListView(ui->catalogueList);
        connect(bookListView, &BookListView::refreashView, this, &MemberView::refreashView);


        // // Adding data to the entry
        QLabel* titleLabel = bookListView->findChild<QLabel*>("titleOutputLabel");
        titleLabel->setText(book["title"].toString());

        QLabel* authorLabel = bookListView->findChild<QLabel*>("authorOutputLabel");
        authorLabel->setText(book["author"].toString());

        QLabel* isbnLabel = bookListView->findChild<QLabel*>("isbnOutputLabel");
        isbnLabel->setText(book["isbn"].toString());

        //setting availability and member options
        if(bookManager->isAvailable(book["isbn"].toString())){
            QLabel* availabilityTextLabel = bookListView->findChild<QLabel*>("availabilityTextLabel");
            availabilityTextLabel->setText("Available");
            QStackedWidget* stackedWidget = bookListView->findChild<QStackedWidget*>("optionsStackedWidget");
            int index = stackedWidget->indexOf(bookListView->findChild<QWidget*>("userAvailablePage"));
            stackedWidget->setCurrentIndex(index);
        } else {
            QLabel* availabilityTextLabel = bookListView->findChild<QLabel*>("availabilityTextLabel");
            availabilityTextLabel->setText("Not Available");
            QStackedWidget* stackedWidget = bookListView->findChild<QStackedWidget*>("optionsStackedWidget");
            int index = stackedWidget->indexOf(bookListView->findChild<QWidget*>("userNotAvailablePage"));
            stackedWidget->setCurrentIndex(index);
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
            qDebug() << "MemberView: Cover label not found";
        }

        //adding entry to list
        QListWidgetItem* item = new QListWidgetItem(ui->catalogueList);

        //adding alternating colors
        if(i % 2 == 0){
            item->setBackground(QBrush(QColor(158,206,104)));
        } else {
            item->setBackground(QBrush(QColor(187,211,180)));
        }

        item->setSizeHint(bookListView->sizeHint());
        ui->catalogueList->setItemWidget(item, bookListView);
    }
}
