#ifndef BOOKMANAGEMENT_H
#define BOOKMANAGEMENT_H

#include "dataManagement.h"

class bookManagement : public DataManagement {

private:

public:
    bookManagement();
    bool addBook(
        const QString& titleInput,
        const QString& authorInput,
        const QString& isbnInput,
        const QString& descInput,
        const QString& genreInput,
        const QString& sectInput);
};

#endif // BOOKMANAGEMENT_H
