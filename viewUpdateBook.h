#ifndef VIEWUPDATEBOOK_H
#define VIEWUPDATEBOOK_H

#include <QDialog>

namespace Ui {
class ViewUpdateBook;
}

class ViewUpdateBook : public QDialog {
    Q_OBJECT

private:
    Ui::ViewUpdateBook *ui;
    QString imageLocation;


    //overrides
    // void dragEnterEvent(QDragEnterEvent *event);
    // void dropEvent(QDropEvent *event);

private slots:
    void updateButtonClicked();

public:
    explicit ViewUpdateBook(QWidget *parent = nullptr);
    ~ViewUpdateBook();
    void initialize(const QString& isbn);
    void populateDetails(const QString& isbn);

signals:
    void updateDisplayRequest();
    void requestRefreash();
};

#endif // VIEWUPDATEBOOK_H
