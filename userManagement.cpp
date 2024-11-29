
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

// --- setters ---
void UserManagement::setCurrentUser(const QJsonObject userObj) {
    currentUser = userObj;
}

void UserManagement::setUserArray(){
    if(libraryDatabase.isEmpty()){
        readData();
    }
    userArray = libraryDatabase["users"].toArray();
}

void UserManagement::clearCurrentUser(){
    currentUser = QJsonObject();
}

// --- getters ---
QJsonObject &UserManagement::getCurrentUser(){
    return currentUser;
}

QJsonArray& UserManagement::getUserArray(){
    return userArray;
}

//get user obj which contains all user details
//parameters: QString of the username to get
//returns: QJsonObject of requested user
QJsonObject UserManagement::getUserObj(const QString& username) {

    //checking user data exists
    if(!libraryDatabase.contains("users") || !libraryDatabase["users"].isArray()){
        qDebug()<< "No user data present";
        return QJsonObject();
    }

    for (int i = 0; userArray.size(); ++i){
        //convert user to object
        QJsonObject user = userArray[i].toObject();

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

// --- methods ---

//check if username exists in system
//paramters: QJsonObject of user details
//returns: true exists, false does not
bool UserManagement::usernameExists(QJsonObject &user){
    for(int i = 0; i < userArray.size(); i++){
        QJsonObject existingUser = userArray[i].toObject();
        if(existingUser["username"].toString() == user["username"].toString()){
            qDebug()<<"UserManagment: Username allready exists in database";
            return true;
        }
    }

    qDebug()<<"UserManagement: User name does not exist in database";
    return false;
}

//add a new user to the existing database
//parameters : QJsonObject containing user details (name,username etc)
//returns : on success true, fail to add false.
bool UserManagement::addUser(QJsonObject& newUser){

    //checking user array is populated
    if(userArray.isEmpty()){
        qDebug()<<"UserManagment: User array empty";
        setUserArray();
    }

    //ensure username has been set to lowercase
    QString username = newUser["username"].toString().toLower();
    newUser["username"] = username;

    //adding background details
    newUser["account"] = createUserID();

    //checking if admin flag has been set, default false if not
    if(!newUser.contains("isAdmin")){
        newUser["isAdmin"] = false;
    }

    //checking if active flag has been set, default false if not
    if(!newUser.contains("isActive")){
        newUser["isActive"] = false;
    }

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

//generate a unique user ID;
//paramters: none
//returns: QString containing unique ID.
QString UserManagement::createUserID() {

    bool idExists=true;
    int userId=0;

    while(idExists){
        userId = (std::rand() % (99999));
        idExists=false;

        for (const QJsonValue &userValue : userArray){
            //convert user to object
            QJsonObject user = userValue.toObject();

            if(userId == user["account"].toInt()){
                idExists=true;
                break;
            }
        }
    }

    qDebug()<<"UserManagement: Generated user id "<<userId;
    return QString::number(userId);
}


//check that username & password cobinatin exits in database
//parameters : QString username & QString password to compare
//returns : true for valid combo, false for invalid combo
bool UserManagement::verifyLogin(const QString& usernameInput, const QString& passwordInput){

    qDebug()<<"UserManagment: Begining Verfication process";

    //checking data has loaded correctly, if missing load the data

    if(userArray.isEmpty()){
        qDebug()<<"UserManagment: Users not loaded";
        return false;
    }

    qDebug()<<"UserManagment: Checking member Database";
    //loop through the user array
    for (int i = 0; i < userArray.size(); i++){
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

//check if user is admin
//parameters: QString containing the username to check
//returns: true (admin) false (member);
bool UserManagement::isAdmin(const QString& usernameInput){
    //checking user data exists
    if(!libraryDatabase.contains("users") || !libraryDatabase["users"].isArray()){
        qDebug()<< "UserManagment: No user data present";
        return false;
    }

    //loop through users checking for match
    for (int i = 0; i < userArray.size(); i++){
        //convert user to object
        QJsonObject user = userArray[i].toObject();

        //get stored detials
        QString storedUsername = user["username"].toString();

        //once username is found check isAdmin
        if(usernameInput==storedUsername){
            return user["isAdmin"].toBool();
        }
    }

    return false;
}

//check user has been set to active in system
//parameters : QString of username to check
//returns : true if active, false if not
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
