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
                             const QString& addressInput,
                             int& userNum){

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
        newUser["ID"] = userNum;


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

int UserManagement::userID(int& userNum) {

generate:
    userNum = (std::rand() % (99999));
    bool valid = false;

        QJsonArray IDArray = jsonData["ID"].toArray();
    for (const QJsonValue &userValue : IDArray){
        //convert user to object
        QJsonObject user = userValue.toObject();

        //get stored detials
        int storedID = user["ID"].toInt();

        //once username is found check isAdmin
        if(userNum == storedID){
            goto generate;
        }
        else {
            valid = true;
        }
    }
    if (valid == true) {
        return userNum;
    }
    return 0;
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
