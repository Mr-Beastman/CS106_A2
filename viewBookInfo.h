#ifndef VIEWBOOKINFO_H
#define VIEWBOOKINFO_H

#include <QDialog>
#include <QJsonObject>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>

namespace Ui {
class ViewBookInfo;
}


class ViewBookInfo : public QDialog {
    Q_OBJECT

private:
    Ui::ViewBookInfo* ui;
    QString currentUser;

private slots:
    void backButtonClicked();
    void checkoutButtonClicked();
    void returnButtonClicked();
    void holdButtonClicked();
    void removeHoldbuttonClicked();
    void confirmButtonClicked();
    void editButtonClicked();
    void refreashBookInfo();

public:
    ViewBookInfo(QWidget* parent = nullptr);
    ~ViewBookInfo();
    void setBookDetails(const QJsonObject& bookDetails);
    void setBookAvailibity(const QJsonObject& book, const QString& username);
    void logoutButtonClicked();
    void hideAdminInfo();
    void showAdminInfo();
    void setCurrentUser(const QString& username);

signals:
    void goBack();
    void logoutRequest();
    void refreashMemberDisplay();
};

#endif // VIEWBOOKINFO_H
