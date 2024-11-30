#ifndef MEMBERINFOVIEW_H
#define MEMBERINFOVIEW_H


#include <QDialog>
#include <QJsonObject>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>

namespace Ui {
class MemberInfoView;
}

class MemberInfoView : public QDialog {
    Q_OBJECT

private:
    Ui::MemberInfoView* ui;


private slots:
    void backButtonClicked();
    void activateButtonClicked();
    void deleteButtonClicked();
    void updateButtonClicked();

public:
    MemberInfoView(QWidget* parent = nullptr);
    ~MemberInfoView();
    bool setMemberDetails(const QJsonObject& userToView);
    void updateDisplay(const QJsonObject& updatedMember);
    void generateCheckedout(const QString account);

signals:
    void goBack();
    void requestUpdateDisplay();
};

#endif // MEMBERINFOVIEW_H
