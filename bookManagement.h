#ifndef BOOKMANAGEMENT_H
#define BOOKMANAGEMENT_H

#include "dataManagement.h"
#include "bookListView.h"

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


class BookManagement : virtual public DataManagement {

protected:


public:

    BookManagement();
    //setters
    void setBookArray();

    //getters
    QJsonArray getBookArray();
    QJsonObject getBookDetails(const QString& isbn);
    //get the bookmanager instance
    static BookManagement* getBookManager();

    //methods
    //vertical format for ease of reading
    BookListView* createBookList(const QJsonObject& book, const QJsonObject& entry);
    bool addBook(
        const QString& titleInput,
        const QString& authorInput,
        const QString& isbnInput,
        const QString& descInput,
        const QString& genreInput,
        const QString& sectInput);
    bool updateBook(const QString& isbn, const QJsonObject& updatedBook);
    bool isAvailable(const QString& isbn);
    QString findCoverPath();
    QString findCoverImage(QString& isbn);
    void clearBookArray();

signals:
    void noUpdates();

};



#endif // BOOKMANAGEMENT_H
