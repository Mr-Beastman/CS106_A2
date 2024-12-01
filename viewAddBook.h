#ifndef VIEWADDBOOK_H
#define VIEWADDBOOK_H

#include <QDialog>

namespace Ui {
class ViewAddBook;
}

class ViewAddBook : public QDialog {
    Q_OBJECT

private:
    Ui::ViewAddBook *ui;
    QString imageLocation;

    //overrides
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

private slots:
    void saveButtonClicked();

public:
    explicit ViewAddBook(QWidget *parent = nullptr);
    ~ViewAddBook();

signals:
    void updateDisplayRequest();
};

#endif // VIEWADDBOOK_H
