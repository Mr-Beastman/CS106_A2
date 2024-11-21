#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>

#include "dataManagement.h"
#include "userManagement.h"
#include "bookManagement.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow *ui;
    QStackedWidget *stackedWidget;

    //manager members
    DataManagement* dataManager;
    UserManagement* userManager;
    bookManagement* bookManager;

private slots:
    void showLogin();
    void userLogin(const QString& username, QString password);
    void showRegister();
    void logOut();

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    //getters
    DataManagement* getDataManager();
    UserManagement* getUserManager();
    bookManagement* getBookManager();

signals:
    void loginFail();
    void notActiveAccount();
};

#endif // MAINWINDOW_H
