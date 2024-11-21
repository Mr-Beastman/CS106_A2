#include "BookListView.h"
#include "ui_BookListView.h"

BookListView::BookListView(QWidget *parent)
    : QWidget(parent),
    ui(new Ui::BookListView)
{
    ui->setupUi(this);  // Initializes the UI components and sets up the layout
}

BookListView::~BookListView()
{
    delete ui;  // Cleans up the UI
}
