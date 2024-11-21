#include "AddBookView.h"
#include "mainWindow.h"
#include "adminView.h"
#include "utilities.h"

#include <QDragEnterEvent>
#include <QMimeData>
#include <QFileDialog>
#include <QImageReader>
#include <QFile>


AddBookView::AddBookView(QWidget *parent) : QDialog(parent), ui(new Ui::AddBookView) {
    ui->setupUi(this);

    // set up title
    this->setWindowTitle("Add Book");
    this->setModal(true);

    connect(ui->saveButton, &QPushButton::clicked, this, &AddBookView::saveButtonClicked);

    //allowing for drag and drop features
    setAcceptDrops(true);
    ui->coverPreviewLabel->setAcceptDrops(true);
}

AddBookView::~AddBookView() {
    delete ui;
}

//logic for users to drag file onto window
void AddBookView::dragEnterEvent(QDragEnterEvent *event){
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
void AddBookView::dropEvent(QDropEvent *event){
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


void AddBookView::saveButtonClicked() {

    //getting to mainwindow
    MainWindow* mainWindow = nullptr;
    QWidget* parentWidget = this->parentWidget();

    while(parentWidget){
        mainWindow = qobject_cast<MainWindow*>(parentWidget);
        if(mainWindow){
            break;
        }
        parentWidget = parentWidget->parentWidget();
    }

    bookManagement* bookManager = mainWindow->getBookManager();
    DataManagement* dataManager = mainWindow->getDataManager();

    qDebug()<<"checking it's here "+dataManager->getFilePath();

    QString title = ui->titleLineEdit->text();
    QString author = ui->authorLineEdit->text();
    QString isbn = ui->isbnLineEdit->text();
    QString genre = ui->genreLineEdit->text();
    QString section = ui->sectionLineEdit->text();
    QString description = ui->descriptionTextEdit->toPlainText();

    //saving the book details
    if(bookManager->addBook(title, author, isbn, description, genre, section)){
        qDebug()<<"Book Added Succesfully";

        //save image if one provided
        if(!imageLocation.isEmpty()){
            QString saveLocation = Utilities::setCoverPath()+isbn+".png";
            if(QFile::copy(imageLocation,saveLocation)){
                qDebug()<<"Image saved Successfully";
            } else {
                qDebug()<<"Failed to save image";
            }
        }

        this->accept();
    } else {
        qDebug()<<"Failed to add book";
    }
}
