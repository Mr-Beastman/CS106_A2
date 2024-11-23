#include "dataManagement.h"


// --------------- private functions ---------------
void DataManagement::setDatabasePath() {
    filePath = (findPath()+"/database/libraryDatabase.json");
}

// --------------- protected fucntions ---------------

//identify file path
//pareamters : none
//returns : string containing folder path
QString DataManagement::findPath() {
    //getting the base directory of the application
    QString baseDirectory = QCoreApplication:: applicationDirPath();

    //moving up directory levels to project folder
    QDir dir(baseDirectory);
    dir.cdUp();
    dir.cdUp();
    dir.cdUp();

    //returning file path
    return dir.path();
}


// --------------- public fucntions ---------------

//constuctor
DataManagement::DataManagement() {}

//setters
void DataManagement::setFileData(const QJsonObject newData) {
    jsonData = newData;
}

//getters
const QJsonObject DataManagement::getFileData() {
    return jsonData;
}

QString DataManagement::getFilePath() {
    return filePath;
}



//reads data from Json file and stores in it a member variable
bool DataManagement::readData(){

    setDatabasePath();

    QFile file(filePath);

    if(!file.exists()){
        qDebug()<<"DataManagement: Database file does not exist.";
        return false;
    }

    if(!file.open(QIODevice::ReadOnly)){
        qDebug()<<"DataManagement: Failed to open database file to read";
        return false;
    }

    QByteArray data = file.readAll();
    QJsonDocument database = QJsonDocument::fromJson(data);

    if(database.isObject()){
        jsonData=database.object();
        file.close();
        return true;
    }

    qDebug()<<"DataManagement: Json file uses an invalid format";
    file.close();
    return false;
}

bool DataManagement::saveData(){
    QFile file(filePath);

    if(!file.open(QIODevice::WriteOnly)){
        qDebug()<<"DataManagement: Failed to open database file for saving";
        return false;
    }

    QJsonDocument doc(jsonData);
    file.write(doc.toJson());
    file.close();
    return true;
}

