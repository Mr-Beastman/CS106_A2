#include "utilities.h"

utilities::utilities() {}

//to find correct file path regardless of where it is installed
//parameters : none
//returns : string containing file path
QString utilities::findPath() {

    //getting the base directory of the application
    QString baseDirectory = QCoreApplication:: applicationDirPath();

    //moving up directory levels to project folder
    QDir dir(baseDirectory);
    dir.cdUp();
    dir.cdUp();
    dir.cdUp();

    QString filePath = dir.path();

    //returning file path
    return filePath;
}

QString utilities::setDatabasePath()
{
    return utilities::findPath()+QDir::separator()+"database"+QDir::separator()+"libraryDatabase.db";
}
