#ifndef UTILITIES_H
#define UTILITIES_H

#include <QCoreApplication>
#include <QDir>

class Utilities
{
public:
    Utilities();

    static QString findPath();
    static QString setDatabasePath();
    static QString setCoverPath(QString coverISBN);
};

#endif // UTILITIES_H
