#ifndef ADMINVIEW_H
#define ADMINVIEW_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QListWidgetItem>
#include <QPixmap>
#include <QListWidget>

#include "ui_adminView.h"

namespace Ui {
class AdminView;
}

class AdminView : public QWidget {

    Q_OBJECT

private:
    Ui::AdminView ui;

private slots:
    void addButtonClicked();

public:
    explicit AdminView(QWidget* parent = nullptr);

    void loadUsers();
    void displayUsers();
};

#endif // ADMINVIEW_H
