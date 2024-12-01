#ifndef VIEWREGISTRATION_H
#define VIEWREGISTRATION_H

#include <QWidget>
#include <managementUser.h>

namespace Ui {
    class ViewRegistration;
}

class ViewRegistration : public QWidget {

    Q_OBJECT

private:
    Ui::ViewRegistration *ui;

private slots:
    void submitButtonClicked();
    void cancelButtonClicked();

public:
    explicit ViewRegistration(QWidget *parent = nullptr);
    ~ViewRegistration();

signals:
    void loginRequest();
};

#endif // VIEWREGISTRATION_H
