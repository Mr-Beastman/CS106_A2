#include "managementUser.h"
#include "managementData.h"

// --------------- private ---------------



// --------------- protected ---------------

//constructor
ManagementUser::ManagementUser() : ManagementData(){
    setUserArray();
}

// --------------- public ---------------

// --- setters ---
void ManagementUser::setCurrentUser(const QString username) {
    currentUser = username;
}

void ManagementUser::setUserArray(){
    QJsonObject& database = getFileData();
    userArray = database["users"].toArray();
}

void ManagementUser::clearCurrentUser(){
    currentUser = QString();
}

// --- getters ---
QString &ManagementUser::getCurrentUser(){
    return currentUser;
}

QJsonArray ManagementUser::getUserArray(){
    return userArray;
}

//get user obj which contains all user details
//parameters: QString of the username to get
//returns: QJsonObject of requested user
QJsonObject ManagementUser::getUserObj(const QString& username) {

    QJsonArray userArray = getUserArray();

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

QJsonObject ManagementUser::getUserObjAccount(const QString &account){
    //checking user data exists

    QJsonArray userArray = getUserArray();

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

QString ManagementUser::getAccount(const QString &username){

    QJsonArray userArray = getUserArray();
    for(int i = 0; i<userArray.size(); i++){
        QJsonObject user = userArray[i].toObject();

        if(user["username"].toString() == username){
            qDebug()<<"ManagementUser: Account Number identified";
            return user["account"].toString();
        }
    }

    qDebug()<<"ManagementUser: Account Number could not be found";
    return QString();
}

// --- methods ---

//check if username exists in system
//paramters: QJsonObject of user details
//returns: true exists, false does not
bool ManagementUser::usernameExists(QJsonObject &user){
    QJsonArray userArray = getUserArray();
    for(int i = 0; i < userArray.size(); i++){
        QJsonObject existingUser = userArray[i].toObject();
        if(existingUser["username"].toString() == user["username"].toString()){
            qDebug()<<"UserManagment: Username allready exists in database";
            return true;
        }
    }

    qDebug()<<"ManagementUser: User name does not exist in database";
    return false;
}

//add a new user to the existing database
//parameters : QJsonObject containing user details (name,username etc)
//returns : on success true, fail to add false.
bool ManagementUser::addUser(QJsonObject& newUser){

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

bool ManagementUser::updateUser(const QString& account, QJsonObject& updatedDetails){
    //check data is laoded
    QJsonArray userArray = getUserArray();
    if (userArray.isEmpty()) {
        qDebug() << "ManagementUser: User array is empty";
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
                    qDebug() << "ManagementUser: User has been updated";
                    return true;
                } else {
                    qDebug() << "ManagementUser: Failed to save updated user data";
                    return false;
                }
            } else {
                qDebug() << "ManagementUser: No changes detected";
                return false;
            }
        }
    }

    qDebug() << "ManagementUser: User not found";
    return false;  // User not found
}

//generate a unique user ID;
//paramters: none
//returns: QString containing unique ID.
//author camryn Hansen, Later modified by Benjamin Eastman
QString ManagementUser::createUserID() {

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

    qDebug()<<"ManagementUser: Generated user id "<<userId;
    return QString::number(userId);
}


//check that username & password combinatin exits in database
//parameters : QString username & QString password to compare
//returns : true for valid combo, false for invalid combo
bool ManagementUser::verifyLogin(const QString& usernameInput, const QString& passwordInput){

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
            qDebug()<<"ManagementUser: Login Successful for user : " << usernameInput;
            return true;
        }
    }

    //if no match found return false and debug
    qDebug()<<"ManagementUser: Login attempt failed for :" << usernameInput;
    return false;
}

//check if user is admin
//parameters: QString containing the username to check
//returns: true (admin) false (member);
bool ManagementUser::isAdmin(const QString& usernameInput){
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
bool ManagementUser::isActive(const QString& usernameInput){
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

bool ManagementUser::activateUser(const QString &accountInput) {
    QJsonArray userArray = getUserArray();
    qDebug()<<"login"<<userArray;
    for (int i = 0; i < userArray.size(); i++) {
        QJsonObject user = userArray[i].toObject();

        if (user["account"].toString() == accountInput) {

            // Check if the user is already active
            if (isActive(accountInput)) {
                qDebug() << "ManagementUser: User already active";
                return true;
            } else {
                qDebug() << "ManagementUser: Activating User";


                user["isActive"] = true;
                userArray[i] = user;


                QJsonObject& fileData = getFileData();
                fileData["users"] = userArray;

                if (saveData()) {
                    qDebug() << "ManagementUser: User has been activated and saved";
                    return true;
                } else {
                    qDebug() << "ManagementUser: Could not save updated user data";
                    return false;
                }
            }
        }
    }

    qDebug() << "ManagementUser: User not found";
    return false;
}

bool ManagementUser::deleteMember(const QString &accountNumber){
    //checking if the user data exists

    if (userArray.isEmpty()) {
        qDebug() << "ManagementUser: User array is empty";
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
                qDebug() << "ManagementUser: User has been deleted";
                return true;
            } else {
                qDebug() << "ManagementUser: Could not save updated user data after deletion";
                return false;
            }
        }
    }

    //if no user found
    qDebug() << "ManagementUser: User not found";
    return false;  // User not found

}
