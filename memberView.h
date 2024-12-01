#ifndef MEMBERVIEW_H
#define MEMBERVIEW_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QListWidgetItem>
#include <QPixmap>
#include <QListWidget>

#include "ui_memberView.h"

namespace Ui {
class MemberView;
}

class MemberView : public QWidget {

    Q_OBJECT

private:
    Ui::MemberView* ui;
    QString accountNumber;

private slots:
    //accountTab related
    void updateButtonClicked();
    void logoutButtonClicked();

public:
    //view related
    explicit MemberView(QWidget* parent = nullptr);
    ~MemberView();

    //setters
    void setAccountNumber(const QString& account);

    //getters
    QString& getAccountNumber();

    //accountTab Related
    void displayCurrentMember();
    void displayCheckedOut();
    void displayHoldRequests();
    void stackedWidgetDisplay(QStackedWidget* toSet, QWidget* page);

    //catalogueTab related
    void loadCatalogue();
    void clearDisplay();

public slots:
    void onBookClicked(QListWidgetItem* book);
    void updateDisplays();

signals:
    void logoutRequest();
    void requestBookInfo(QJsonObject& bookDetails);
    void refreashMemberDisplay();
};



#endif // MEMBERVIEW_H
