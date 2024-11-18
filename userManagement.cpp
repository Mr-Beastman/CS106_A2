#include "userManagement.h"
#include "dataManagement.h"

UserManagement::UserManagement(const QString& dbPath) : DataManagement(){
    readData();
}

bool UserManagement::verifyLogin(const QString& usernameInput, const QString& passwordInput){

    //checking user data exists
    if(!jsonData.contains("users") || !jsonData["users"].isArray()){
        qDebug()<< "No user data present";
        return false;
    }

    //adding all users to array
    QJsonArray usersArray = jsonData["users"].toArray();

    //loop through users checking for match
    for (const QJsonValue &userValue : usersArray){
        //convert user to object
        QJsonObject user = userValue.toObject();

        //get stored detials
        QString storedUsername = user["username"].toString();
        QString storedPassword = user["password"].toString();

        //compare inputs and stored values
        //if match found return true and debug
        if(usernameInput==storedUsername && passwordInput == storedPassword){
            qDebug()<<"Login Successful for user : "<<usernameInput;
            return true;
        }
    }

    //if no match found return false and debug
    qDebug()<<"Login attempt failed";
    return false;
}

bool UserManagement::isActive(){
    return true;
}

bool UserManagement::isAdmin(const QString& usernameInput){
    //checking user data exists
    if(!jsonData.contains("users") || !jsonData["users"].isArray()){
        qDebug()<< "No user data present";
        return false;
    }

    //adding all users to array
    QJsonArray usersArray = jsonData["users"].toArray();

    //loop through users checking for match
    for (const QJsonValue &userValue : usersArray){
        //convert user to object
        QJsonObject user = userValue.toObject();

        //get stored detials
        QString storedUsername = user["username"].toString();

        //once username is found check isAdmin
        if(usernameInput==storedUsername){
            return user["isAdmin"].toBool();
        }
    }

    return false;
}

bool UserManagement::addUser(){
    return true;
}
