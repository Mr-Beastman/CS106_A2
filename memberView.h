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



private slots:
    //accountTab related
    void updateButtonClicked();
    void logoutButtonClicked();

public:
    //view related
    explicit MemberView(QWidget* parent = nullptr);
    ~MemberView();



    //accountTab Related
    void displayCurrentMember();
    void displayCheckedOut();
    void displayHoldRequests();
    void stackedWidgetDisplay(QStackedWidget* toSet, QWidget* page);

    //catalogueTab related
    void loadCatalogue();

public slots:
    void onBookClicked(QListWidgetItem* book);
    void refreashMemberView();

signals:
    void logoutRequest();
    void requestBookInfo(QJsonObject& bookDetails);

};

#endif // MEMBERVIEW_H
