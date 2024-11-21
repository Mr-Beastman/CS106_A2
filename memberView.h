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
    void refreashView();
public:
    //viewRelated functions
    explicit MemberView(QWidget* parent = nullptr);
    ~MemberView();
    void displayCurrentMember(const QJsonObject& currentUser);


};

#endif // MEMBERVIEW_H
