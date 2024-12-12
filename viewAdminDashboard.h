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

private slots:
    void addButtonClicked();
    void addMemberButtonClicked();
    void logoutButtonClicked();

public:
    explicit ViewAdminDashboard(QWidget* parent = nullptr);

    void displayAdminCatalogue();
    void displayUsers();
    void updateDisplays();
    void onMemberClicked(QListWidgetItem *user);
    void onBookClicked(QListWidgetItem *book);

signals:
    void logoutRequest();
    void requestBookInfo(QJsonObject& bookDetails);
    void requestMemberInfo(QJsonObject& bookDetails);
};

#endif // VIEWADMINDASHBOARD_H
