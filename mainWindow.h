#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QProcess>

#include "managementData.h"
#include "managementUser.h"
#include "managementBook.h"
#include "managementTransaction.h"

#include "viewLogin.h"
#include "viewRegistration.h"
#include "viewAdminDashboard.h"
#include "viewMemberDashboard.h"
#include "viewBookInfo.h"
#include "viewMemberInfo.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow *ui;
    QStackedWidget *stackedWidget;

    //page members
    ViewLogin* loginPage;
    ViewRegistration* registrationPage;
    ViewAdminDashboard* adminPage;
    ViewMemberDashboard* memberPage;
    ViewBookInfo* bookInfoPage;
    ViewMemberInfo* memberInfoPage;

private slots:
    void showLogin();
    void userLogin(const QString& username, QString password);
    void showRegister();
    void showBookInfo(QJsonObject& bookDetails);
    void showMemberInfo(QJsonObject& userToView);
    void updateAdminDisplays();
    void updateMemberDisplays();
    void goBack();
    void logOut();

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void loadData();

signals:
    void loginFail();
    void notActiveAccount();
};

#endif // MAINWINDOW_H
