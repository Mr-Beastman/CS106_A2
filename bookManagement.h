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

private:
    //setting up singtleton
    //static ptr to bookmanager instance
    static BookManagement* bookManager;
    static QMutex bookMtx;

protected:
    BookManagement();

public:
    //enforcing singleton
    //removing copy contructor to prevent copies
    BookManagement(const BookManagement& obj) = delete;
    //removing assigment operator to prevent assignment
    BookManagement& operator=(const BookManagement& obj) = delete;

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
