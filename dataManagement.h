#ifndef DATAMANAGEMENT_H
#define DATAMANAGEMENT_H

#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QCoreApplication>
#include <QDebug>
#include <QMutex>

class DataManagement {


protected:
    QJsonObject libraryDatabase;

    QString filePath;
    void setDatabasePath();
    QString findPath();

public:
    DataManagement();

    //getters
    QString getFilePath();
    QJsonObject& getFileData();

    //methods
    bool readData();
    bool saveData();
    void clearData();
};

#endif // DATAMANAGEMENT_H
