
#include "managementUser.h"
#include "managementData.h"

// --------------- private ---------------



// --------------- protected ---------------

//constructor
managementUser::managementUser() : ManagementData(){
    setUserArray();
}

// --------------- public ---------------

// --- setters ---
void managementUser::setCurrentUser(const QString username) {
    currentUser = username;
}

void managementUser::setUserArray(){
    QJsonObject& database = getFileData();
    userArray = database["users"].toArray();
}

void managementUser::clearCurrentUser(){
    currentUser = QString();
}

// --- getters ---
QString &managementUser::getCurrentUser(){
    return currentUser;
}

QJsonArray managementUser::getUserArray(){
    return userArray;
}

//get user obj which contains all user details
//parameters: QString of the username to get
//returns: QJsonObject of requested user
QJsonObject managementUser::getUserObj(const QString& username) {

    QJsonObject& database = getFileData();
    QJsonArray userArray = database["users"].toArray();

    for (int i = 0; i<userArray.size(); ++i){
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

QJsonObject managementUser::getUserObjAccount(const QString &account){
    //checking user data exists

    QJsonObject& database = getFileData();
    QJsonArray userArray = database["user"].toArray();

    for (int i = 0; i<userArray.size(); ++i){
        //convert user to object
        QJsonObject user = userArray[i].toObject();

        //on matching username
        if(user["account"].toString()==account){
            return user;
        }
    }

    //display error and return empty object
    qDebug()<<"UserManagment: User not found";
    return QJsonObject();
}

QString managementUser::getAccount(const QString &username){

    QJsonArray userArray = getUserArray();
    for(int i = 0; i<userArray.size(); i++){
        QJsonObject user = userArray[i].toObject();

        if(user["username"].toString() == username){
            qDebug()<<"managementUser: Account Number identified";
            return user["account"].toString();
        }
    }

    qDebug()<<"managementUser: Account Number could not be found";
    return QString();
}

// --- methods ---

//check if username exists in system
//paramters: QJsonObject of user details
//returns: true exists, false does not
bool managementUser::usernameExists(QJsonObject &user){
    QJsonArray userArray = getUserArray();
    for(int i = 0; i < userArray.size(); i++){
        QJsonObject existingUser = userArray[i].toObject();
        if(existingUser["username"].toString() == user["username"].toString()){
            qDebug()<<"UserManagment: Username allready exists in database";
            return true;
        }
    }

    qDebug()<<"managementUser: User name does not exist in database";
    return false;
}

//add a new user to the existing database
//parameters : QJsonObject containing user details (name,username etc)
//returns : on success true, fail to add false.
bool managementUser::addUser(QJsonObject& newUser){

    //checking user array is populated
    QJsonArray userArray = getUserArray();
    if(userArray.isEmpty()){
        qDebug()<<"UserManagment: User array empty";
        //setUserArray();
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

bool managementUser::updateUser(const QString& account, QJsonObject& updatedDetails){
    //check data is laoded
    QJsonArray userArray = getUserArray();
    if (userArray.isEmpty()) {
        qDebug() << "managementUser: User array is empty";
        //setUserArray();
    }

    //search for match
    for (int i = 0; i < userArray.size(); ++i) {
        QJsonObject user = userArray[i].toObject();

        //if found
        if (user["account"].toString() == account) {
            //flag to check for updates
            bool updated = false;

            //check if anything has been updated
            if (user["name"].toString() != updatedDetails["name"].toString()) {
                user["name"] = updatedDetails["name"].toString();
                updated = true;
            }
            if (user["phone"].toString() != updatedDetails["phone"].toString()) {
                user["phone"] = updatedDetails["phone"].toString();
                updated = true;
            }
            if (user["email"].toString() != updatedDetails["email"].toString()) {
                user["email"] = updatedDetails["email"].toString();
                updated = true;
            }
            if (user["address"].toString() != updatedDetails["address"].toString()) {
                user["address"] = updatedDetails["address"].toString();
                updated = true;
            }

            //if there are updates
            if (updated) {

                //update database
                userArray[i] = user;
                QJsonObject& fileData = getFileData();
                fileData["users"] = userArray;

                //save
                if (saveData()) {
                    qDebug() << "managementUser: User has been updated";
                    return true;
                } else {
                    qDebug() << "managementUser: Failed to save updated user data";
                    return false;
                }
            } else {
                qDebug() << "managementUser: No changes detected";
                return false;
            }
        }
    }

    qDebug() << "managementUser: User not found";
    return false;  // User not found
}

//generate a unique user ID;
//paramters: none
//returns: QString containing unique ID.
QString managementUser::createUserID() {

    bool idExists=true;
    int userId=0;

    while(idExists){
        userId = (std::rand() % (99999));
        idExists=false;
        QJsonArray userArray = getUserArray();
        for (int i = 0; i<userArray.size(); i++){
            //convert user to object
            QJsonObject user = userArray[i].toObject();

            if(userId == user["account"].toInt()){
                idExists=true;
                break;
            }
        }
    }

    qDebug()<<"managementUser: Generated user id "<<userId;
    return QString::number(userId);
}


//check that username & password cobinatin exits in database
//parameters : QString username & QString password to compare
//returns : true for valid combo, false for invalid combo
bool managementUser::verifyLogin(const QString& usernameInput, const QString& passwordInput){

    qDebug()<<"UserManagment: Begining Verfication process";

    if(userArray.isEmpty()){
        qDebug()<<"UserManagment: Users not loaded";
        return false;
    }

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
            qDebug()<<"managementUser: Login Successful for user : " << usernameInput;
            return true;
        }
    }

    //if no match found return false and debug
    qDebug()<<"managementUser: Login attempt failed for :" << usernameInput;
    return false;
}

//check if user is admin
//parameters: QString containing the username to check
//returns: true (admin) false (member);
bool managementUser::isAdmin(const QString& usernameInput){
    //checking user data exists
    if(!libraryDatabase.contains("users") || !libraryDatabase["users"].isArray()){
        qDebug()<< "UserManagment: No user data present";
        return false;
    }
    QJsonArray userArray = getUserArray();
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
bool managementUser::isActive(const QString& usernameInput){
    QJsonArray users = getUserArray();

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

bool managementUser::activateUser(const QString &accountInput) {
    QJsonArray userArray = getUserArray();
    qDebug()<<"login"<<userArray;
    for (int i = 0; i < userArray.size(); i++) {
        QJsonObject user = userArray[i].toObject();

        if (user["account"].toString() == accountInput) {

            // Check if the user is already active
            if (isActive(accountInput)) {
                qDebug() << "managementUser: User already active";
                return true;
            } else {
                qDebug() << "managementUser: Activating User";


                user["isActive"] = true;
                userArray[i] = user;


                QJsonObject& fileData = getFileData();
                fileData["users"] = userArray;

                if (saveData()) {
                    qDebug() << "managementUser: User has been activated and saved";
                    return true;
                } else {
                    qDebug() << "managementUser: Could not save updated user data";
                    return false;
                }
            }
        }
    }

    qDebug() << "managementUser: User not found";
    return false;
}

bool managementUser::deleteMember(const QString &accountNumber){
    //checking if the user data exists
    QJsonArray userArray = getUserArray();
    if (userArray.isEmpty()) {
        qDebug() << "managementUser: User array is empty";
        //setUserArray();
    }

    //finding user to delete
    for (int i = 0; i < userArray.size(); ++i) {
        QJsonObject user = userArray[i].toObject();

        //if found
        if (user["account"].toString() == accountNumber) {
            //removing user from array
            userArray.removeAt(i);

            //update database
            QJsonObject& fileData = getFileData();
            fileData["users"] = userArray;

            if (saveData()) {
                qDebug() << "managementUser: User has been deleted";
                return true;
            } else {
                qDebug() << "managementUser: Could not save updated user data after deletion";
                return false;
            }
        }
    }

    //if no user found
    qDebug() << "managementUser: User not found";
    return false;  // User not found

}

// void managementUser::updateUserArray(QJsonArray array)
// {
//     QJsonArray userArray = getUserArray();
//     userArray = array;
// }

// void managementUser::clearUserArray(){
//     userArray= QJsonArray();
// }
