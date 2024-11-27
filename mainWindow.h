#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>

#include "dataManagement.h"
#include "userManagement.h"
#include "bookManagement.h"
#include "transactionManagement.h"

#include "loginView.h"
#include "registrationView.h"
#include "adminView.h"
#include "memberView.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow *ui;
    QStackedWidget *stackedWidget;

    //manager members to store intial instance.
    DataManagement* dataManager;
    UserManagement* userManager;
    BookManagement* bookManager;
    TransactionManagement* transactionManager;

    //page members
    LoginView* loginPage;
    RegistrationView* registrationPage;
    AdminView* adminPage;
    MemberView* memberPage;

private slots:
    void showLogin();
    void userLogin(const QString& username, QString password);
    void showRegister();
    void logOut();

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


signals:
    void loginFail();
    void notActiveAccount();
};

#endif // MAINWINDOW_H
