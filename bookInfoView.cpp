#include "bookInfoView.h"
#include "bookManagement.h"
#include "ui_bookInfoView.h"
#include "userManagement.h"



BookInfoView::BookInfoView(QWidget* parent) : QDialog(parent), ui(new Ui::BookInfoView) {
    ui->setupUi(this);

    //connect buttons
    connect(ui->backButton, &QPushButton::clicked, this, &BookInfoView::backButtonClicked);
}

BookInfoView::~BookInfoView() {
    delete ui;
}

void BookInfoView::backButtonClicked() {
    emit goBack();
}

void BookInfoView::setBookDetails(const QJsonObject &bookDetails){

    ui->titleLabel->setText(bookDetails["title"].toString());
    ui->authorOutputLabel->setText(bookDetails["author"].toString());
    ui->isbnOutputLabel->setText(bookDetails["isbn"].toString());
    ui->bookDescription->setPlainText(bookDetails["desc"].toString());

    // adding cover image
    BookManagement* bookManager = BookManagement::getBookManager();
    QString coverImagePath = bookManager->findCoverPath() + bookDetails["isbn"].toString() + ".png";
    QPixmap cover(coverImagePath);
    QString noCoverImagePath = bookManager->findCoverPath() + "noCover.png";
    QPixmap noCover(noCoverImagePath);

    if (!cover.isNull()) {
        ui->coverLabel->setPixmap(cover);
    } else {
        ui->coverLabel->setPixmap(noCover);
    }

    qDebug()<<"BookInfoView: Book Info View Generated";
}
