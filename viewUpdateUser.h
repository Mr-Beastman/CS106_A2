#ifndef VIEWUPDATEUSER_H
#define VIEWUPDATEUSER_H

#include <QDialog>


namespace Ui {
class ViewUpdateUser;
}

class ViewUpdateUser : public QDialog {
    Q_OBJECT

private:
    QString accountNumber;
    Ui::ViewUpdateUser *ui;

private slots:
    void saveButtonClicked();

public:
    explicit ViewUpdateUser(QWidget *parent = nullptr);
    ~ViewUpdateUser();

    //setter
    void setAccountNumber(const QString& account);

    //getter
    QString getAccountNumber();

    //methods
    void preloadUser(const QString& account);

signals:
    void requestRefreash();
};

#endif // VIEWUPDATEUSER_H
