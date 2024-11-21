#ifndef ADDBOOKVIEW_H
#define ADDBOOKVIEW_H

#include <QDialog>

#include "ui_addBookView.h"

namespace Ui {
class AddBookView;
}

class AddBookView : public QDialog {
    Q_OBJECT

private:
    Ui::AddBookView *ui;
    QString imageLocation;

    //overrides
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

private slots:
    void saveButtonClicked();

public:
    explicit AddBookView(QWidget *parent = nullptr);
    ~AddBookView();
};

#endif // ADDBOOKVIEW_H
