#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>

#include "userManagement.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow *ui;
    QStackedWidget *stackedWidget;
    UserManagement *userManager;

private slots:
    void showLogin();
    void userLogin(const QString& username, QString password);
    void showRegister();

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void loginFail();
};

#endif // MAINWINDOW_H
