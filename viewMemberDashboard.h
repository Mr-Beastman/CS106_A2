#ifndef VIEWMEMBERDASHBOARD_H
#define VIEWMEMBERDASHBOARD_H

#include "qstackedwidget.h"
#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QListWidgetItem>
#include <QPixmap>
#include <QListWidget>


namespace Ui {
class ViewMemberDashboard;
}

class ViewMemberDashboard : public QWidget {

    Q_OBJECT

private:
    Ui::ViewMemberDashboard* ui;
    QString accountNumber;
    QString username;

private slots:
    //accountTab related
    void updateButtonClicked();
    void logoutButtonClicked();

public:
    //view related
    explicit ViewMemberDashboard(QWidget* parent = nullptr);
    ~ViewMemberDashboard();

    //setters
    void setAccountNumber(const QString& account);
    void setUsername(const QString& account);

    //getters
    QString& getAccountNumber();

    //accountTab Related
    void displayCurrentMember();
    void displayCheckedOut();
    void displayHoldRequests();
    void stackedWidgetDisplay(QStackedWidget* toSet, QWidget* page);

    //catalogueTab related
    void displayCatalogue();
    void clearDisplay();

public slots:
    void onBookClicked(QListWidgetItem* book);
    void updateDisplays();

signals:
    void logoutRequest();
    void requestBookInfo(QJsonObject& book, const QString& username);
    void refreashMemberDisplay();
};



#endif // VIEWMEMBERDASHBOARD_H
