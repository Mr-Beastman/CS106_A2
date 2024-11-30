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
    void clearCurrentUser();

    //getters
    QJsonObject& getCurrentUser();
    QJsonArray getUserArray();
    QJsonObject getUserObj(const QString &username);
    QJsonObject getUserObjAccount(const QString &account);
    QString getAccount(const QString& username);
    //get the bookmanager instance
    static UserManagement* getUserManager();

    //methods
    void updateCurrentUser();
    bool usernameExists(QJsonObject& user);
    bool addUser(QJsonObject &newUser);
    bool updateUser(const QString& account, QJsonObject& updatedDetails);
    QString createUserID();
    bool verifyLogin(const QString& usernameInput, const QString& passwordInput);
    bool isAdmin(const QString& usernameInput);
    bool isActive(const QString &usernameInput);
    bool activateUser(const QString& usernameInput);
    bool deleteMember(const QString& accountNumber);
    void updateUserArray(QJsonArray array);
    void clearUserArray();
};

#endif // USERMANAGEMENT_H
