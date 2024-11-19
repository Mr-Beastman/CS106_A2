#ifndef REGISTRATIONVIEW_H
#define REGISTRATIONVIEW_H

#include <QWidget>
#include "userManagement.h"

namespace Ui {
    class RegistrationView;
}

class RegistrationView : public QWidget {

    Q_OBJECT

private:
    Ui::RegistrationView *ui;
    UserManagement* userManager;

private slots:
    void submitButtonClicked();
    void cancelButtonClicked();

public:
    explicit RegistrationView(UserManagement* userManager, QWidget *parent = nullptr);
    ~RegistrationView();

signals:
    void loginRequest();
    void submitNewUser(
        const QString& nameInput,
        const QString& usernameInput,
        const QString& passwordInput,
        const QString& phoneInput,
        const QString& emailInput,
        const QString& addressInput);
};

#endif // REGISTRATIONVIEW_H
