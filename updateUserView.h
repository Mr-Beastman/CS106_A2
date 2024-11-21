#ifndef UPDATEUSERVIEW_H
#define UPDATEUSERVIEW_H

#include <QDialog>

#include "ui_updateUserView.h"

namespace Ui {
class UpdateUserView;
}

class UpdateUserView : public QDialog {
    Q_OBJECT

private:
    Ui::UpdateUserView *ui;

private slots:
    void preloadUser();
    void saveButtonClicked();

public:
    explicit UpdateUserView(QWidget *parent = nullptr);
    ~UpdateUserView();

signals:
    void requestRefreash();
};

#endif // UPDATEUSERVIEW_H
