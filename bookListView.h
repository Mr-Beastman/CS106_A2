#ifndef BOOKLISTVIEW_H
#define BOOKLISTVIEW_H

#include <QWidget>
#include <QTimer>

namespace Ui {
class BookListView;
}

class BookListView : public QWidget {

    Q_OBJECT

private:
    Ui::BookListView *ui;

private slots:
    void checkoutBook();
    void placeHold();
    void returnButtonClicked();
    void removeHoldButtonClicked();


public:
    explicit BookListView(QWidget *parent = nullptr);
    ~BookListView();
    void editBook();

signals:
    void refreashView();
    void refreashMemberView();

};

#endif // BOOKLISTVIEW_H
