#include "viewUpdateBook.h"
#include "ui_viewUpdateBook.h"

#include "managementBook.h"

#include <QDragEnterEvent>
#include <QMimeData>
#include <QFileDialog>
#include <QImageReader>
#include <QFile>

// --------------- constructor ---------------
ViewUpdateBook::ViewUpdateBook(QWidget *parent) : QDialog(parent), ui(new Ui::ViewUpdateBook) {
    ui->setupUi(this); // Initialize UI from the generated class

    // Set up window
    this->setWindowTitle("Modify Book Details");
    this->setModal(true);

    //connect button
    connect(ui->updateButton, &QPushButton::clicked, this, &ViewUpdateBook::updateButtonClicked);

    //allowing for drag and drop features
    setAcceptDrops(true);
    ui->coverPreviewLabel->setAcceptDrops(true);
}

// --------------- destructor ---------------
ViewUpdateBook::~ViewUpdateBook() {
    delete ui;
}


// --------------- private ---------------

void ViewUpdateBook::populateDetails(const QString &isbn){

    qDebug()<<"Pop:"<<isbn;
    ManagementBook bookManager;
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

//logic for users to drag file onto window
void ViewUpdateBook::dragEnterEvent(QDragEnterEvent *event){
    if(event->mimeData()->hasUrls()){
        auto urls = event->mimeData()->urls();
        //checking file is image
        for(const QUrl &url : urls) {
            QString fileLocation = url.toLocalFile();
            QImageReader reader(fileLocation);
            if(!reader.format().isEmpty()){
                event->acceptProposedAction();
                return;
            }
        }
    }
    event->ignore();
}

//logic for users to drop file onto window
void ViewUpdateBook::dropEvent(QDropEvent *event){
    auto urls = event->mimeData()->urls();
    if(!urls.empty()){
        QString fileLocation = urls.first().toLocalFile();
        QImageReader reader(fileLocation);
        if(!reader.format().isEmpty()){
            //diplay a preview
            QPixmap pixmap(fileLocation);
            ui->coverPreviewLabel->setPixmap(pixmap.scaled(ui->coverPreviewLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }

        //storing file path
        this->imageLocation=fileLocation;
    }
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

    ManagementBook bookManager;

    //save image if one provided
    if(!imageLocation.isEmpty()){
        QString saveLocation = bookManager.findCoverPath()+ui->isbnLineEdit->text()+".png";

        //checking if file already exists
        if(QFile::exists(saveLocation)){
            //delete exsiting if it does
            QFile::remove(saveLocation);
        }

        //save updated version
        if(QFile::copy(imageLocation,saveLocation)){
            qDebug()<<"viewAddBook: Image saved Successfully";
        } else {
            qDebug()<<"viewAddBook: Failed to save image";
        }
    }

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


