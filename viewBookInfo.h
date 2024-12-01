#ifndef VIEWBOOKINFO_H
#define VIEWBOOKINFO_H

#include <QDialog>
#include <QJsonObject>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>

namespace Ui {
class ViewBookInfo;
}


class ViewBookInfo : public QDialog {
    Q_OBJECT

private:
    Ui::ViewBookInfo* ui;

private slots:
    void backButtonClicked();

public:
    ViewBookInfo(QWidget* parent = nullptr);
    ~ViewBookInfo();
    void setBookDetails(const QJsonObject& bookDetails);

signals:
    void goBack();
};

#endif // VIEWBOOKINFO_H
