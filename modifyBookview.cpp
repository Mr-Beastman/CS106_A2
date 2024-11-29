#include "modifyBookview.h"
#include "ui_modifyBookView.h"

#include "bookManagement.h"

// --------------- constructor ---------------
ModifyBookView::ModifyBookView(QWidget *parent) : QDialog(parent), ui(new Ui::ModifyBookView) {
    ui->setupUi(this); // Initialize UI from the generated class

    // Set up window
    this->setWindowTitle("Modify Book Details");
    this->setModal(true);

    //connect button
    connect(ui->updateButton, &QPushButton::clicked, this, &ModifyBookView::updateButtonClicked);
}

// --------------- destructor ---------------
ModifyBookView::~ModifyBookView() {
    delete ui;
}


// --------------- private ---------------

void ModifyBookView::populateDetails(const QString &isbn){

    qDebug()<<"Pop:"<<isbn;
    BookManagement* bookManager = BookManagement::getBookManager();
    QJsonObject bookDetails = bookManager->getBookDetails(isbn);

    if(bookDetails.isEmpty()){
        qDebug()<<"ModifyBookView: Cannot find book detials for "<<isbn;
        return;
    }

    ui->titleLineEdit->setText(bookDetails["title"].toString());
    ui->authorLineEdit->setText(bookDetails["author"].toString());
    ui->isbnLineEdit->setText(bookDetails["isbn"].toString());
    ui->genreLineEdit->setText(bookDetails["genre"].toString());
    ui->sectionLineEdit->setText(bookDetails["sect"].toString());
    ui->descriptionTextEdit->setText(bookDetails["desc"].toString());
}

// --------------- private slots ---------------

void ModifyBookView::updateButtonClicked(){
    //getting updates from ui
    QJsonObject updatedBook;

    qDebug()<<"ModifyBookView: Storing Updates";
    updatedBook["title"] = ui->titleLineEdit->text();
    updatedBook["author"]= ui->authorLineEdit->text();
    updatedBook["isbn"] = ui->isbnLineEdit->text();
    updatedBook["genre"] = ui->genreLineEdit->text();
    updatedBook["sect"] = ui->sectionLineEdit->text();
    updatedBook["desc"] = ui->descriptionTextEdit->toPlainText();

    BookManagement* bookManager = BookManagement::getBookManager();

    qDebug()<<"ModifyBookView: Starting update process";
    if(bookManager->updateBook(ui->isbnLineEdit->text(), updatedBook)){
        qDebug()<<"ModifyBookView: Requesting display refreash";
        emit requestRefreash();
        accept();
    }
}


// --------------- public methods ---------------


void ModifyBookView::initialize(const QString &isbn) {

    qDebug()<<"init:"<<isbn;
    if(!isbn.isEmpty()){
        populateDetails(isbn);
    } else {
        qDebug()<<"ModifyBookView: Isbn is empty. Cannon load details";
    }
}


