#ifndef VIEWADMINDASHBOARD_H
#define VIEWADMINDASHBOARD_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QListWidgetItem>
#include <QPixmap>
#include <QListWidget>

#include "ui_viewAdminDashboard.h"

namespace Ui {
class ViewAdminDashboard;
}

class ViewAdminDashboard : public QWidget {

    Q_OBJECT

private:
    Ui::ViewAdminDashboard* ui;

    QString adminUser;

private slots:
    void addButtonClicked();
    void addMemberButtonClicked();
    void logoutButtonClicked();

public:
    explicit ViewAdminDashboard(QWidget* parent = nullptr);

    //setters
    void setAdminUser(const QString& username);

    //getters
    QString getAdminUser();

    void displayAdminCatalogue();
    void displayUsers();
    void updateDisplays();
    void onMemberClicked(QListWidgetItem *user);
    void onBookClicked(QListWidgetItem *book);

signals:
    void logoutRequest();
    void requestBookInfo(QJsonObject& bookDetails, const QString& username);
    void requestMemberInfo(QJsonObject& bookDetails, const QString& adminUser);
};

#endif // VIEWADMINDASHBOARD_H
