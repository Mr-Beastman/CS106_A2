#ifndef REGISTRATIONVIEW_H
#define REGISTRATIONVIEW_H

#include <QWidget>
#include <userManagement.h>

namespace Ui {
    class RegistrationView;
}

class RegistrationView : public QWidget {

    Q_OBJECT

private:
    Ui::RegistrationView *ui;

private slots:
    void submitButtonClicked();
    void cancelButtonClicked();

public:
    explicit RegistrationView(QWidget *parent = nullptr);
    ~RegistrationView();

signals:
    void loginRequest();
};

#endif // REGISTRATIONVIEW_H
