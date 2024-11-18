#include "registrationView.h"
#include "ui_registrationView.h"

RegistrationView::RegistrationView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RegistrationView)
{
    ui->setupUi(this);

    connect(ui->cancelButton, &QPushButton::clicked, this, &RegistrationView::cancelButtonClicked);
}

RegistrationView::~RegistrationView()
{
    delete ui;
}


void RegistrationView::cancelButtonClicked()
{
    emit loginRequest();
}
