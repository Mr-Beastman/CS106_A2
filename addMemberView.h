#ifndef ADDMEMBERVIEW_H
#define ADDMEMBERVIEW_H

#include <QDialog>

namespace Ui {
class AddMemberView;
}

class AddMemberView : public QDialog {
    Q_OBJECT

private:
    Ui::AddMemberView *ui;

private slots:
    void addMemberButtonClicked();

public:
    explicit AddMemberView(QWidget *parent = nullptr);
    ~AddMemberView();

signals:
    void updateDisplayRequest();
};

#endif // ADDMEMBERVIEW_H
