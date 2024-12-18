#ifndef ManagementUser_H
#define ManagementUser_H

#include "managementData.h"

class ManagementUser : virtual public ManagementData {

private:

protected:
    QString currentUser;
    QJsonArray userArray;

public:
    ManagementUser();
    //setters
    void setCurrentUser(const QString username);
    void setUserArray();


    //getters
    QString& getCurrentUser();
    QJsonArray getUserArray();
    QJsonObject getUserObj(const QString &username);
    QJsonObject getUserObjAccount(const QString &account);
    QString getAccount(const QString& username);


    //methods
    bool usernameExists(QJsonObject& user);
    bool addUser(QJsonObject &newUser);
    bool updateUser(const QString& account, QJsonObject& updatedDetails);
    QString createUserID();
    bool verifyLogin(const QString& usernameInput, const QString& passwordInput);
    bool isAdmin(const QString& usernameInput);
    bool isActive(const QString &usernameInput);
    bool activateUser(QJsonObject &userAccount);
    bool deleteUser(const QString& accountNumber);
    void clearCurrentUser();
};

#endif // ManagementUser_H
