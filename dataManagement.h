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

private:
    //setting up singtleton
    //static ptr to datamanager instance
    static DataManagement* dataManager;
    static QMutex dataMtx;

protected:
    DataManagement();
    QJsonObject libraryDatabase;
    QString filePath;
    void setDatabasePath();
    QString findPath();

public:
    //enforcing singleton
    //removing copy contructor to prevent copies
    DataManagement(const DataManagement& obj) = delete;
    //removing assigment operator to prevent assignment
    DataManagement& operator=(const DataManagement& obj) = delete;

    //get the dataManager instance
    static DataManagement* getDataManager();


    //setters

    //getters
    QString getFilePath();
    QJsonObject& getFileData();

    //methods
    bool readData();
    bool saveData();
    void clearData();
};

#endif // DATAMANAGEMENT_H
