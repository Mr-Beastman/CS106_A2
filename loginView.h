#ifndef LOGINVIEW_H
#define LOGINVIEW_H

#include <QWidget>
#include "ui_loginView.h"

class LoginView : public QWidget
{
    Q_OBJECT

private:
    Ui::LoginView ui;

private slots:
    void loginButtonClicked();
    void registerButtonClicked();
    void loginError();
    void notActiveError();

public:
    explicit LoginView(QWidget* parent = nullptr);
    void clearError();
    void clearInputs();

signals:
    void callRegisterView();
    void loginAttempt(const QString& username, const QString& password);
};

#endif // LOGINVIEW_H
