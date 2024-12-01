#ifndef VIEWMEMBERINFO_H
#define VIEWMEMBERINFO_H


#include <QDialog>
#include <QJsonObject>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>

namespace Ui {
class ViewMemberInfo;
}

class ViewMemberInfo : public QDialog {
    Q_OBJECT

private:
    Ui::ViewMemberInfo* ui;


private slots:
    void backButtonClicked();
    void activateButtonClicked();
    void deleteButtonClicked();
    void updateButtonClicked();

public:
    ViewMemberInfo(QWidget* parent = nullptr);
    ~ViewMemberInfo();
    bool setMemberDetails(const QJsonObject& userToView);
    void updateDisplay(const QJsonObject& updatedMember);
    void generateCheckedout(const QString account);

signals:
    void goBack();
    void requestUpdateDisplay();
};

#endif // VIEWMEMBERINFO_H
