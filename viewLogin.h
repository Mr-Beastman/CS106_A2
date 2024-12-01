#ifndef VIEWLOGIN_H
#define VIEWLOGIN_H

#include <QWidget>
#include "ui_viewLogin.h"

namespace Ui {
class ViewLogin;
}

class ViewLogin : public QWidget
{
    Q_OBJECT

private:
    Ui::ViewLogin ui;

private slots:
    void loginButtonClicked();
    void registerButtonClicked();
    void loginError();
    void notActiveError();

public:
    explicit ViewLogin(QWidget* parent = nullptr);
    void clearError();
    void clearInputs();

signals:
    void callRegisterView();
    void loginAttempt(const QString& username, const QString& password);
};

#endif // VIEWLOGIN_H
