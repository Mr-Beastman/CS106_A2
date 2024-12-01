#include "managementData.h"


// --------------- private functions ---------------

//constuctor
ManagementData::ManagementData() {}


void ManagementData::setDatabasePath() {
    filePath = (findPath()+"/database/libraryDatabase.json");
}

// --------------- protected fucntions ---------------

//identify file path
//pareamters : none
//returns : string containing folder path
QString ManagementData::findPath() {
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

//setters

//getters
QJsonObject& ManagementData::getFileData() {
    if(libraryDatabase.isEmpty()){
        readData();
    }
    return libraryDatabase;
}


//reads data from Json file and stores in it a member variable
bool ManagementData::readData(){

    setDatabasePath();

    QFile file(filePath);

    if(!file.exists()){
        qDebug()<<"ManagementData: Database file does not exist.";
        libraryDatabase=QJsonObject();
        return false;
    }

    if(!file.open(QIODevice::ReadOnly)){
        qDebug()<<"ManagementData: Failed to open database file to read";
        libraryDatabase=QJsonObject();
        return false;
    }

    QByteArray data = file.readAll();
    QJsonDocument database = QJsonDocument::fromJson(data);

    if(database.isObject()){
        libraryDatabase=database.object();
        file.close();
        return true;
    }

    qDebug()<<"ManagementData: Json file uses an invalid format";
    libraryDatabase=QJsonObject();
    file.close();
    return false;
}

bool ManagementData::saveData(){
    setDatabasePath();

    QFile file(filePath);

    if(!file.open(QIODevice::WriteOnly)){
        qDebug()<<"ManagementData: Failed to open database file for saving";
        return false;
    }

    QJsonDocument doc(libraryDatabase);

    file.write(doc.toJson());
    file.close();
    return true;
}

void ManagementData::clearData(){
    libraryDatabase=QJsonObject();
}

