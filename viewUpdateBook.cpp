#include "viewUpdateBook.h"
#include "ui_viewUpdateBook.h"

#include "managementBook.h"

#include <QDragEnterEvent>
#include <QMimeData>
#include <QFileDialog>
#include <QImageReader>
#include <QFile>
#include <QMessageBox>

// --------------- constructor ---------------
ViewUpdateBook::ViewUpdateBook(QWidget *parent) : QDialog(parent), ui(new Ui::ViewUpdateBook) {
    ui->setupUi(this);

    // Set up window
    this->setWindowTitle("Modify Book Details");
    this->setModal(true);

    //connect button
    connect(ui->updateButton, &QPushButton::clicked, this, &ViewUpdateBook::updateButtonClicked);
    connect(ui->deleteButton, &QPushButton::clicked, this, &ViewUpdateBook::deleteButtonClicked);
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

    ManagementBook bookManager;
    QJsonObject bookDetails = bookManager.getBookDetails(isbn);

    if(bookDetails.isEmpty()){
        qDebug()<<"viewUpdateBook: Cannot find book detials for "<<isbn;
        return;
    }

    ui->titleLineEdit->setText(bookDetails["title"].toString());
    ui->authorLineEdit->setText(bookDetails["author"].toString());
    ui->genreLineEdit->setText(bookDetails["genre"].toString());
    ui->sectionLineEdit->setText(bookDetails["sect"].toString());
    ui->descriptionTextEdit->setText(bookDetails["desc"].toString());
    ui->storedIsbnLabel->setText(bookDetails["isbn"].toString());
    ui->storedIsbnLabel->hide();
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
    updatedBook["genre"] = ui->genreLineEdit->text();
    updatedBook["sect"] = ui->sectionLineEdit->text();
    updatedBook["desc"] = ui->descriptionTextEdit->toPlainText();

    ManagementBook bookManager;

    //save image if one provided
    if(!imageLocation.isEmpty()){
        QString saveLocation = bookManager.findCoverPath()+ui->storedIsbnLabel->text()+".png";

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

    bool updated = bookManager.updateBook(ui->storedIsbnLabel->text(), updatedBook);

    if(updated || !imageLocation.isEmpty()){
        qDebug()<<"viewUpdateBook: Requesting display refreash";
        emit requestRefreash();
        accept();
    } else {
        ui->errorLabel->setText("No Updates Identified");
    }
}

void ViewUpdateBook::deleteButtonClicked(){
    ManagementBook bookManager;

    if(bookManager.isIssued(ui->storedIsbnLabel->text()) || bookManager.hasHolds(ui->storedIsbnLabel->text())){
        qDebug()<<"ViewUpdateBook: Cant delete book due to activity";
        QMessageBox::warning(this,tr("Delete Denied"),tr("Book currently has active loans and/or holds. Please remove these first."), QMessageBox::Ok);
    } else {
        qDebug()<<"ViewUpdateBook: Book can be deleted";
        if(bookManager.deleteBook(ui->storedIsbnLabel->text())){
            emit requestRefreash();
            accept();
        }
    }
}


// --------------- public methods ---------------


