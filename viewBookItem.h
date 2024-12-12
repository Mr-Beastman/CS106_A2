#ifndef VIEWBOOKITEM_H
#define VIEWBOOKITEM_H

#include <QWidget>
#include <QTimer>

namespace Ui {
class ViewBookItem;
}

class ViewBookItem : public QWidget {

    Q_OBJECT

private:
    Ui::ViewBookItem *ui;

private slots:
    void checkoutBook();
    void placeHold();
    void returnButtonClicked();
    void removeHoldButtonClicked();
    void confirmButtonClicked();


public:
    explicit ViewBookItem(QWidget *parent = nullptr);
    ~ViewBookItem();
    void editBook();

signals:
    void refreashView();
    void refreashviewMemberDashboard();

};

#endif // VIEWBOOKITEM_H
