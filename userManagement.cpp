
#include "userManagement.h"
#include "dataManagement.h"

// --------------- private ---------------

//intializing static memebers
UserManagement * UserManagement::userManager = nullptr;
QMutex UserManagement::userMtx;


// --------------- protected ---------------

//constructor
UserManagement::UserManagement() : DataManagement(){}

// --------------- public ---------------

//setters
void UserManagement::setCurrentUser(const QJsonObject userObj) {
    currentUser = userObj;
}

void UserManagement::setUserArray(){
    if(libraryDatabase.isEmpty()){
        readData();
    }
    userArray = libraryDatabase["users"].toArray();
}

QJsonObject UserManagement::updateCurrentUser(const QString &username){
    for(int i = 0; i<userArray.size(); i++){
        QJsonObject user = userArray[i].toObject();

        if(user["username"].toString() == username){
            return user;
        }
    }

    qDebug()<<"UserManagement: User not found";
    return QJsonObject();
}

void UserManagement::clearCurrentUser(){
    currentUser = QJsonObject();
}

//getters
QJsonObject &UserManagement::getCurrentUser(){
    return currentUser;
}

QJsonArray& UserManagement::getUserArray(){
    return userArray;
}

QJsonObject UserManagement::getUserObj(const QString& username) {

    //checking user data exists
    if(!libraryDatabase.contains("users") || !libraryDatabase["users"].isArray()){
        qDebug()<< "No user data present";
        return QJsonObject();
    }

    //adding all users to array
    QJsonArray usersArray = libraryDatabase["users"].toArray();

    for (const QJsonValue &userValue : usersArray){
        //convert user to object
        QJsonObject user = userValue.toObject();

        //on matching username
        if(user["username"].toString()==username){
            return user;
        }
    }

    //display error and return empty object
    qDebug()<<"UserManagment: User not found";
    return QJsonObject();
}

//getter for bookManager singleton
UserManagement *UserManagement::getUserManager() {
    if(userManager == nullptr) {
        QMutexLocker locker(&userMtx);
        if(userManager == nullptr){
            userManager = new UserManagement();
        }
    }
    return userManager;
}

bool UserManagement::updateUserInArray(){

    if(currentUser.empty()){
        qDebug()<<"UserMangamer: Current user empty";
        return false;
    }

    bool updated = false;

    for(int i = 0; i<userArray.size(); i++){
        QJsonObject user = userArray[i].toObject();

        if(user["username"].toString() == currentUser["username"].toString()){
            userArray.replace(i, currentUser);
            updated = true;
            break;
        }
    }

    if(!updated){
        qDebug()<<"UserManagement: Current user not in array";
        return false;
    }

    return true;
}

//methods

bool UserManagement::verifyLogin(const QString& usernameInput, const QString& passwordInput){

    qDebug()<<"UserManagment: Begining Verfication process";

    //checking data has loaded correctly, if missing load the data

    if(userArray.isEmpty()){
        qDebug()<<"UserManagment: Users not loaded";
        return false;
    }

    //loop through the user array
    for (int i = 0; userArray.size(); i++){
        //convert user to object
        QJsonObject user = userArray[i].toObject();

        //get stored detials
        QString storedUsername = user["username"].toString();
        QString storedPassword = user["password"].toString();

        //compare inputs and stored values
        //if match found return true and debug
        if(usernameInput==storedUsername && passwordInput == storedPassword){
            qDebug()<<"UserManagement: Login Successful for user : " << usernameInput;
            return true;
        }
    }

    //if no match found return false and debug
    qDebug()<<"UserManagement: Login attempt failed for :" << usernameInput;
    return false;
}

bool UserManagement::addUser(const QString& nameInput,
                             const QString& usernameInput,
                             const QString& passwordInput,
                             const QString& phoneInput,
                             const QString& emailInput,
                             const QString& addressInput){

    //checking user array is populated
    if(userArray.isEmpty()){
        qDebug()<<"UserManagment: User array empty";
        setUserArray();
    }

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
    newUser["account"] = userID();

    //add newUser to the existing array
    userArray.append(newUser);
    libraryDatabase["users"] = userArray;

    //save updates to json file
    if(saveData()){
        qDebug()<<"UserManagment: New user has been added";
        return true;
    }

    qDebug()<<"UserManagment: Failed to save new user";
    return false;
}

bool UserManagement::isActive(const QString& usernameInput){
    QJsonArray& users = getUserArray();

    //loop through users checking for match
    for (int i = 0; i < users.size(); i++){
        //convert user to object
        QJsonObject user = users[i].toObject();

        //get stored detials
        QString storedUsername = user["username"].toString();

        //once username is found check isAdmin
        if(usernameInput==storedUsername){
            return user["isActive"].toBool();
        }
    }

    return false;
}

bool UserManagement::isAdmin(const QString& usernameInput){
    //checking user data exists
    if(!libraryDatabase.contains("users") || !libraryDatabase["users"].isArray()){
        qDebug()<< "UserManagment: No user data present";
        return false;
    }

    //adding all users to array
    QJsonArray usersArray = libraryDatabase["users"].toArray();

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

int UserManagement::userID() {

generate:
    int userNum = (std::rand() % (99999));
    bool valid = false;

    QJsonArray IDArray = libraryDatabase["users"].toArray();
    for (const QJsonValue &userValue : IDArray){
        //convert user to object
        QJsonObject user = userValue.toObject();

        //get stored detials
        int storedID = user["account"].toInt();

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
    return userNum;
}
