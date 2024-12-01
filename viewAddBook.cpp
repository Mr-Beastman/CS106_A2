#include "viewAddBook.h"
#include "managementBook.h"
#include "ui_viewAddBook.h"

#include <QDragEnterEvent>
#include <QMimeData>
#include <QFileDialog>
#include <QImageReader>
#include <QFile>


ViewAddBook::ViewAddBook(QWidget *parent) : QDialog(parent), ui(new Ui::ViewAddBook) {
    ui->setupUi(this);

    // set up title
    this->setWindowTitle("Add Book");
    this->setModal(true);

    connect(ui->saveButton, &QPushButton::clicked, this, &ViewAddBook::saveButtonClicked);

    //allowing for drag and drop features
    setAcceptDrops(true);
    ui->coverPreviewLabel->setAcceptDrops(true);
}

ViewAddBook::~ViewAddBook() {
    delete ui;
}

//logic for users to drag file onto window
void ViewAddBook::dragEnterEvent(QDragEnterEvent *event){
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
void ViewAddBook::dropEvent(QDropEvent *event){
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


void ViewAddBook::saveButtonClicked() {

    managementBook bookManager;

    QString title = ui->titleLineEdit->text();
    QString author = ui->authorLineEdit->text();
    QString isbn = ui->isbnLineEdit->text();
    QString genre = ui->genreLineEdit->text();
    QString section = ui->sectionLineEdit->text();
    QString description = ui->descriptionTextEdit->toPlainText();

    //saving the book details
    if(bookManager.addBook(title, author, isbn, description, genre, section)){
        qDebug()<<"viewAddBook: Book Added Succesfully";

        //save image if one provided
        if(!imageLocation.isEmpty()){
            QString saveLocation = bookManager.findCoverPath()+isbn+".png";
            if(QFile::copy(imageLocation,saveLocation)){
                qDebug()<<"viewAddBook: Image saved Successfully";
            } else {
                qDebug()<<"viewAddBook: Failed to save image";
            }
        }

        //send signal to dashboards to update
        emit updateDisplayRequest();

        this->accept();
    } else {
        qDebug()<<"viewAddBook: Failed to add book";
    }
}
