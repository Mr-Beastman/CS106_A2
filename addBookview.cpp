#include "AddBookView.h"

AddBookView::AddBookView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddBookView)
{
    ui->setupUi(this);

    // set up title
    this->setWindowTitle("Add Book");
    this->setModal(true);
}

AddBookView::~AddBookView()
{
    delete ui;
}
