#ifndef ADDBOOKVIEW_H
#define ADDBOOKVIEW_H

#include <QDialog>

#include "ui_addBookView.h"
#include "bookManagement.h"

namespace Ui {
class AddBookView;
}

class AddBookView : public QDialog
{
    Q_OBJECT

private:
    Ui::AddBookView *ui;

private slots:
    void saveButtonClicked();

public:
    explicit AddBookView(QWidget *parent = nullptr);
    ~AddBookView();
};

#endif // ADDBOOKVIEW_H
