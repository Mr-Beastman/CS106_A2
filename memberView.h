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
    void displayCurrentMember(const QJsonObject& currentUser);
    void displayCheckedOut(const QJsonObject& currentUser);
    void displayHoldRequests(const QJsonObject& currentUser);

    //catalogueTab related
    void loadCatalogue();

public slots:
    void refreashView();

signals:
    void logoutRequest();

};

#endif // MEMBERVIEW_H
