#ifndef ManagementData_H
#define ManagementData_H

#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QCoreApplication>
#include <QDebug>
#include <QMutex>

class ManagementData {

private:
    QString filePath;

protected:
    QJsonObject libraryDatabase;
    QString findPath();
    void setDatabasePath();

public:
    //constructor
    ManagementData();

    //getters
    QJsonObject& getFileData();

    //methods
    bool readData();
    bool saveData();
    void clearData();
};

#endif // ManagementData_H
