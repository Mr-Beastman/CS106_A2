#ifndef ADDBOOKVIEW_H
#define ADDBOOKVIEW_H

#include <QDialog>

#include "ui_addBookView.h"

namespace Ui {
class AddBookView;
}

class AddBookView : public QDialog
{
    Q_OBJECT

public:
    explicit AddBookView(QWidget *parent = nullptr);
    ~AddBookView();

private:
    Ui::AddBookView *ui;
};

#endif // ADDBOOKVIEW_H
