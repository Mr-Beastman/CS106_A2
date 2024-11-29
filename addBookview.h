#ifndef ADDBOOKVIEW_H
#define ADDBOOKVIEW_H

#include <QDialog>

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

signals:
    void updateDisplayRequest();
};

#endif // ADDBOOKVIEW_H
