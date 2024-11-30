#ifndef UPDATEUSERVIEW_H
#define UPDATEUSERVIEW_H

#include <QDialog>

#include "ui_updateUserView.h"

namespace Ui {
class UpdateUserView;
}

class UpdateUserView : public QDialog {
    Q_OBJECT

private:
    QString accountNumber;
    Ui::UpdateUserView *ui;

private slots:
    void saveButtonClicked();

public:
    explicit UpdateUserView(QWidget *parent = nullptr);
    ~UpdateUserView();

    //setter
    void setAccountNumber(const QString& account);

    //getter
    QString getAccountNumber();

    //methods
    void preloadUser(const QString& account);

signals:
    void requestRefreash();
};

#endif // UPDATEUSERVIEW_H
