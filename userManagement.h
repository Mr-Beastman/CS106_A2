#ifndef USERMANAGEMENT_H
#define USERMANAGEMENT_H

#include "dataManagement.h"

class UserManagement : virtual public DataManagement {

private:

protected:
    QString currentUser;

public:
    UserManagement();
    //setters
    void setCurrentUser(const QString username);
    void setUserArray();
    void clearCurrentUser();

    //getters
    QString& getCurrentUser();
    QJsonArray getUserArray();
    QJsonObject getUserObj(const QString &username);
    QJsonObject getUserObjAccount(const QString &account);
    QString getAccount(const QString& username);
    //get the usermanager instance
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
