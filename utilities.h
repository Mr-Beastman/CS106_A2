#ifndef UTILITIES_H
#define UTILITIES_H

#include <QCoreApplication>
#include <QDir>

class utilities
{
public:
    utilities();

    static QString findPath();
    static QString setDatabasePath();
};

#endif // UTILITIES_H
