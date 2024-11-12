#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "userLogin.h"

#include <QMainWindow>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_loginButton_clicked();

    void on_registerButton_clicked();

private:
    Ui::MainWindow *ui;
    userVerification *userVerify;

    void passLogins();
};
#endif // MAINWINDOW_H
