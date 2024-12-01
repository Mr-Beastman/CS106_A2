#ifndef BOOKINFOVIEW_H
#define BOOKINFOVIEW_H

#include <QDialog>
#include <QJsonObject>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>

namespace Ui {
class BookInfoView;
}


class BookInfoView : public QDialog {
    Q_OBJECT

private:
    Ui::BookInfoView* ui;

private slots:
    void backButtonClicked();

public:
    BookInfoView(QWidget* parent = nullptr);
    ~BookInfoView();
    void setBookDetails(const QJsonObject& bookDetails);

signals:
    void goBack();
};

#endif // BOOKINFOVIEW_H
