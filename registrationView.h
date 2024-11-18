#ifndef REGISTRATIONVIEW_H
#define REGISTRATIONVIEW_H

#include <QWidget>

namespace Ui {
    class RegistrationView;
}

class RegistrationView : public QWidget {

    Q_OBJECT

public:
    explicit RegistrationView(QWidget *parent = nullptr);
    ~RegistrationView();

signals:
    void loginRequest();

private slots:
    void cancelButtonClicked();

private:
    Ui::RegistrationView *ui;
};

#endif // REGISTRATIONVIEW_H
