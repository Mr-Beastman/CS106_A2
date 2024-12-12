#ifndef ManagementBook_H
#define ManagementBook_H

#include "ManagementData.h"
#include "viewBookItem.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QMutex>
#include <QMutexLocker>
#include <QListWidgetItem>
#include <QStackedWidget>
#include <QLabel>
#include <QPixmap>
#include <QBrush>
#include <QColor>
#include <QDir>


class ManagementBook : virtual public ManagementData {

protected:
    QJsonArray bookArray;


public:

    ManagementBook();
    //setters
    void setBookArray();

    //getters
    QJsonArray getBookArray();
    QJsonObject getBookDetails(const QString& isbn);

    //methods
    //vertical format for ease of reading
    ViewBookItem* createBookList(const QJsonObject& book, const QJsonObject& entry);
    bool addBook(
        const QString& titleInput,
        const QString& authorInput,
        const QString& isbnInput,
        const QString& descInput,
        const QString& genreInput,
        const QString& sectInput);
    bool updateBook(const QString& isbn, const QJsonObject& updatedBook);
    bool isAvailable(const QString& isbn);
    bool isIssued(const QString& isbn);
    QString findCoverPath();
    QString findCoverImage(QString& isbn);
    void clearBookArray();

signals:
    void noUpdates();

};



#endif // ManagementBook_H
