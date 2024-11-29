#ifndef MODIFYBOOKVIEW_H
#define MODIFYBOOKVIEW_H

#include <QDialog>

namespace Ui {
class ModifyBookView;
}

class ModifyBookView : public QDialog {
    Q_OBJECT

private:
    Ui::ModifyBookView *ui;
    QString imageLocation;


    //overrides
    // void dragEnterEvent(QDragEnterEvent *event);
    // void dropEvent(QDropEvent *event);

private slots:
    void updateButtonClicked();

public:
    explicit ModifyBookView(QWidget *parent = nullptr);
    ~ModifyBookView();
    void initialize(const QString& isbn);
    void populateDetails(const QString& isbn);

signals:
    void updateDisplayRequest();
    void requestRefreash();
};

#endif // MODIFYBOOKVIEW_H
