#include "viewUpdateBook.h"
#include "ui_viewUpdateBook.h"

#include "managementBook.h"

// --------------- constructor ---------------
ViewUpdateBook::ViewUpdateBook(QWidget *parent) : QDialog(parent), ui(new Ui::ViewUpdateBook) {
    ui->setupUi(this); // Initialize UI from the generated class

    // Set up window
    this->setWindowTitle("Modify Book Details");
    this->setModal(true);

    //connect button
    connect(ui->updateButton, &QPushButton::clicked, this, &ViewUpdateBook::updateButtonClicked);
}

// --------------- destructor ---------------
ViewUpdateBook::~ViewUpdateBook() {
    delete ui;
}


// --------------- private ---------------

void ViewUpdateBook::populateDetails(const QString &isbn){

    qDebug()<<"Pop:"<<isbn;
    managementBook bookManager;
    QJsonObject bookDetails = bookManager.getBookDetails(isbn);

    if(bookDetails.isEmpty()){
        qDebug()<<"viewUpdateBook: Cannot find book detials for "<<isbn;
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

void ViewUpdateBook::updateButtonClicked(){
    //getting updates from ui
    QJsonObject updatedBook;

    qDebug()<<"viewUpdateBook: Storing Updates";
    updatedBook["title"] = ui->titleLineEdit->text();
    updatedBook["author"]= ui->authorLineEdit->text();
    updatedBook["isbn"] = ui->isbnLineEdit->text();
    updatedBook["genre"] = ui->genreLineEdit->text();
    updatedBook["sect"] = ui->sectionLineEdit->text();
    updatedBook["desc"] = ui->descriptionTextEdit->toPlainText();

    managementBook bookManager;

    qDebug()<<"viewUpdateBook: Starting update process";
    if(bookManager.updateBook(ui->isbnLineEdit->text(), updatedBook)){
        qDebug()<<"viewUpdateBook: Requesting display refreash";
        emit requestRefreash();
        accept();
    }
}


// --------------- public methods ---------------


void ViewUpdateBook::initialize(const QString &isbn) {

    qDebug()<<"init:"<<isbn;
    if(!isbn.isEmpty()){
        populateDetails(isbn);
    } else {
        qDebug()<<"viewUpdateBook: Isbn is empty. Cannon load details";
    }
}


