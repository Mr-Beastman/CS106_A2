#include "AddBookView.h"
#include "mainWindow.h"
#include "adminView.h"


AddBookView::AddBookView(QWidget *parent) : QDialog(parent), ui(new Ui::AddBookView) {
    ui->setupUi(this);

    // set up title
    this->setWindowTitle("Add Book");
    this->setModal(true);

    connect(ui->saveButton, &QPushButton::clicked, this, &AddBookView::saveButtonClicked);
}

AddBookView::~AddBookView() {
    delete ui;
}

void AddBookView::saveButtonClicked() {

    AdminView* adminView = dynamic_cast <AdminView*>(parent());
    MainWindow* mainWindow = dynamic_cast<MainWindow*>(adminView->parent());

    bookManagement* bookManager = mainWindow->getBookManager();

    QString title = ui->titleLineEdit->text();
    QString author = ui->authorLineEdit->text();
    QString isbn = ui->isbnLineEdit->text();
    QString genre = ui->genreLineEdit->text();
    QString section = ui->sectionLineEdit->text();
    QString description = ui->descriptionTextEdit->toPlainText();

    if(bookManager->addBook(title, author, isbn, description, genre, section)){
        qDebug()<<"Book Added Succesfully";
        this->accept();
    } else {
        qDebug()<<"Failed to add book";
    }
}
