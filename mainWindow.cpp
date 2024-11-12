#include "mainWindow.h"
#include "userLogin.h"
#include "ui_mainWindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , userVerify(new userVerification)
{
    ui->setupUi(this);

    //pass inputs for verfication
    userVerify->setUsername(ui->usernameInput);
    userVerify->setPassword(ui->passwordInput);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete userVerify;
}

void MainWindow::on_loginButton_clicked()
{
    //checking if login details are valid
    if(userVerify->userLogin()){
        //if they are checking user type to direct
        ui->lineEdit_3->setText("Login Succesful");
        if(userVerify->isAdmin()){
            //for testing
            qDebug()<<"admin";
        }
        else{
            //for testing
            qDebug()<<"member";
            if(userVerify->isActive()){
                qDebug()<<"Account is live";
            } else{
                qDebug()<<"Requires verification";
            }
        }
    }
    else{
        //error provided for incorrect details
       ui->lineEdit_3->setText("Invalid username or password");
    }
}

void MainWindow::on_registerButton_clicked()
{

}

