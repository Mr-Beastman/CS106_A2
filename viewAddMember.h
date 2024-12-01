#ifndef viewAddMember_H
#define viewAddMember_H

#include <QDialog>

namespace Ui {
class ViewAddMember;
}

class ViewAddMember : public QDialog {
    Q_OBJECT

private:
    Ui::ViewAddMember *ui;

private slots:
    void addMemberButtonClicked();

public:
    explicit ViewAddMember(QWidget *parent = nullptr);
    ~ViewAddMember();

signals:
    void updateDisplayRequest();
};

#endif // viewAddMember_H
