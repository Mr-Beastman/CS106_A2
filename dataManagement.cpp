#include "dataManagement.h"
#include "utilities.h"

DataManagement::DataManagement() {
    filePath=Utilities::setDatabasePath();
}

bool DataManagement::readData(){
    QFile file(filePath);

    if(!file.open(QIODevice::ReadOnly)){
        qDebug()<<"Failed to open database file";
        return false;
    }

    QByteArray data = file.readAll();
    QJsonDocument database = QJsonDocument::fromJson(data);

    if(database.isObject()){
        jsonData=database.object();
        file.close();
        return true;
    }

    qDebug()<<"Invalid Json format";
    return false;
}

bool DataManagement::saveData(){
    QFile file(filePath);

    if(!file.open(QIODevice::ReadOnly)){
        qDebug()<<"Failed to open database file for saving";
        return false;
    }

    QJsonDocument doc(jsonData);
    file.write(doc.toJson());
    file.close();
    return true;
}

