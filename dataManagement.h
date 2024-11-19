#ifndef DATAMANAGEMENT_H
#define DATAMANAGEMENT_H

#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QJsonDocument>
#include <QDebug>

class DataManagement {
protected:
    QJsonObject jsonData;
    QString filePath;

public:

    //getters
    const QJsonObject getFileData();

    DataManagement();
    bool readData();
    bool saveData();
};

#endif // DATAMANAGEMENT_H
