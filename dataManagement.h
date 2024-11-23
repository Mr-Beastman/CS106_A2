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
    void setDatabasePath();

protected:
    QJsonObject jsonData;
    QString filePath;

    QString findPath();

public:

    //setters
    void setFileData(const QJsonObject newData);

    //getters
    QString getFilePath();
    const QJsonObject getFileData();

    DataManagement();
    bool readData();
    bool saveData();
};

#endif // DATAMANAGEMENT_H
