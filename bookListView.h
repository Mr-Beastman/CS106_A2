#ifndef BOOKLISTVIEW_H
#define BOOKLISTVIEW_H

#include <QWidget>

namespace Ui {
class BookListView;
}

class BookListView : public QWidget
{
    Q_OBJECT

private:
    Ui::BookListView *ui;

public:
    explicit BookListView(QWidget *parent = nullptr);
    ~BookListView();

};

#endif // BOOKLISTVIEW_H
