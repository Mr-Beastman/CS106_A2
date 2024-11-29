#ifndef USERMANAGEMENT_H
#define USERMANAGEMENT_H

#include "dataManagement.h"

class UserManagement : virtual public DataManagement {

private:
    //setting up singtleton
    //static ptr to bookmanager instance
    static UserManagement* userManager;
    static QMutex userMtx;

protected:
    UserManagement();
    QJsonArray userArray;
    QJsonObject currentUser;

public:
    //enforcing singleton
    //removing copy contructor to prevent copies
    UserManagement(const UserManagement& obj) = delete;
    //removing assigment operator to prevent assignment
    UserManagement& operator=(const UserManagement& obj) = delete;

    //setters
    void setCurrentUser(const QJsonObject userObj);
    void setUserArray();
    QJsonObject updateCurrentUser(const QString& username);
    void clearCurrentUser();

    //getters
    QJsonObject& getCurrentUser();
    QJsonArray& getUserArray();
    QJsonObject getUserObj(const QString &username);
    //get the bookmanager instance
    static UserManagement* getUserManager();

    //methods
    bool updateUserInArray();
    bool verifyLogin(const QString& usernameInput, const QString& passwordInput);
    //vertical format for ease of reading
    bool addUser(QJsonObject &newUser);
    bool isAdmin(const QString& usernameInput);
    bool isActive(const QString &usernameInput);
    bool usernameExists(QJsonObject& user);
    int userID();
};

#endif // USERMANAGEMENT_H
