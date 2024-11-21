#include "Utilities.h"

Utilities::Utilities() {}

//to find correct file path regardless of where it is installed
//parameters : none
//returns : string containing file path
QString Utilities::findPath() {

    //getting the base directory of the application
    QString baseDirectory = QCoreApplication:: applicationDirPath();

    //moving up directory levels to project folder
    QDir dir(baseDirectory);
    dir.cdUp();
    dir.cdUp();
    dir.cdUp();


    qDebug()<<"Ultilites : Directory Path "<<dir.path();
    //returning file path
    return dir.path();
}

//get the correct file path for the database
//parameters : none
//returns : string containing database path
QString Utilities::setDatabasePath()
{
    qDebug()<<"Ultilites: Setting database path to "<<(Utilities::findPath()+"/database/libraryDatabase.json");
    return (Utilities::findPath()+"/database/libraryDatabase.json");
}

//get the correct file path for the book covers
//parameters : none
//returns : string containing cover folder path
QString Utilities::setCoverPath(QString coverISBN)
{
    return Utilities::findPath()+QDir::separator()+"coverImages"+QDir::separator()+coverISBN;
}
