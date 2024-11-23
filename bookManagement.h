#ifndef BOOKMANAGEMENT_H
#define BOOKMANAGEMENT_H

#include "dataManagement.h"

class BookManagement : public DataManagement {

private:
    BookManagement();
    ~BookManagement();

    //setting up singtleton
    //static ptr to bookmanager instance
    static BookManagement* bookManager;
    static QMutex bookMtx;

public:
    //enforcing singleton
    //removing copy contructor to prevent copies
    BookManagement(const BookManagement& obj) = delete;
    //removing assigment operator to prevent assignment
    BookManagement& operator=(const BookManagement& obj) = delete;

    //get the bookmanager instance
    static BookManagement* getBookManager();

    bool addBook(
        const QString& titleInput,
        const QString& authorInput,
        const QString& isbnInput,
        const QString& descInput,
        const QString& genreInput,
        const QString& sectInput);

    //functions related to saving & getting cover images
    QString findCoverPath();
    QString findCoverImage(QString& isbn);

};



#endif // BOOKMANAGEMENT_H
