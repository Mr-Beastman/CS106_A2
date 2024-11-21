#include "userManagement.h"
#include "dataManagement.h"

UserManagement::UserManagement() : DataManagement(){
    readData();
}

//setters
void UserManagement::setCurrentUser(const QString usernameInput) {
    //checking user data exists
    if(!jsonData.contains("users") || !jsonData["users"].isArray()){
        qDebug()<< "No user data present";
        return;
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
            currentUser = user;
            return;
        }
    }

    qDebug()<<"UserManagement: Could not store current user";
    return;
}

//getters
QJsonObject UserManagement::getCurrentUser(){
    return currentUser;
}

bool UserManagement::verifyLogin(const QString& usernameInput, const QString& passwordInput){

    qDebug()<<"UserManagemnt: Begining User Verfication";

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

bool UserManagement::isActive(const QString& usernameInput){
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
            return user["isActive"].toBool();
        }
    }

    return false;
}

bool UserManagement::addUser(const QString& nameInput,
                             const QString& usernameInput,
                             const QString& passwordInput,
                             const QString& phoneInput,
                             const QString& emailInput,
                             const QString& addressInput){

        //creating Json obj with new user details;
        QJsonObject newUser;
        newUser["name"] = nameInput;
        newUser["username"] = usernameInput;
        newUser["password"] = passwordInput;
        newUser["phone"] = phoneInput;
        newUser["email"] = emailInput;
        newUser["address"] = addressInput;
        newUser["isAdmin"] = false;
        newUser["isActive"] = false;

        //add newUser to the existing array
        QJsonArray userArray = jsonData["users"].toArray();
        userArray.append(newUser);
        jsonData["users"] =userArray;

        //save updates to json file
        if(saveData()){
            qDebug()<<"New user has been added";
            return true;
        }

        qDebug()<<"Failed to save new user";
        return false;
    }

QJsonObject UserManagement::getUser(const QString& username) {

    //checking user data exists
    if(!jsonData.contains("users") || !jsonData["users"].isArray()){
        qDebug()<< "No user data present";
        return QJsonObject();
    }

    //adding all users to array
    QJsonArray usersArray = jsonData["users"].toArray();

    for (const QJsonValue &userValue : usersArray){
        //convert user to object
        QJsonObject user = userValue.toObject();

        //on matching username
        if(user["username"].toString()==username){
            return user;
        }
    }

    //display error and return empty object
    qDebug()<<"User not found";
    return QJsonObject();
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
