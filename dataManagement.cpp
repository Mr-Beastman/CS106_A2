#include "dataManagement.h"
#include "utilities.h"

//setters
void DataManagement::setFileData(const QJsonObject newData){
    jsonData = newData;
}

//getters
const QJsonObject DataManagement::getFileData() {
    return jsonData;
}

DataManagement::DataManagement() {}

bool DataManagement::readData(){

    filePath = Utilities::setDatabasePath();

    qDebug()<<"adding to qFile path :"<<filePath;

    QFile file(filePath);

    if(!file.exists()){
        qDebug()<<"DataManagement: Database file does not exist.";
        return false;
    }

    if(!file.open(QIODevice::ReadOnly)){
        qDebug()<<"Failed to open database file to read";
        return false;
    }

    QByteArray data = file.readAll();
    QJsonDocument database = QJsonDocument::fromJson(data);

    if(database.isObject()){
        jsonData=database.object();
        qDebug()<<"JSON data stored";
        file.close();
        return true;
    }

    qDebug()<<"Invalid Json format";
    return false;
}

bool DataManagement::saveData(){
    QFile file(filePath);

    if(!file.open(QIODevice::WriteOnly)){
        qDebug()<<"Failed to open database file for saving";
        return false;
    }

    QJsonDocument doc(jsonData);
    file.write(doc.toJson());
    file.close();
    return true;
}

