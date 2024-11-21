#ifndef USERMANAGEMENT_H
#define USERMANAGEMENT_H

#include "dataManagement.h"

class UserManagement : public DataManagement
{
protected:
    QJsonObject currentUser;

public:
    UserManagement();

    //setters
    void setCurrentUser(const QString usernameInput);

    //getters
    QJsonObject getCurrentUser();
    QJsonObject getUser(const QString &username);

    int userID(int& userNum);
    bool verifyLogin(const QString& usernameInput, const QString& passwordInput);
    bool isAdmin(const QString& usernameInput);
    bool isActive(const QString &usernameInput);
    bool addUser(
        const QString& nameInput,
        const QString& usernameInput,
        const QString& passwordInput,
        const QString& phoneInput,
        const QString& emailInput,
        const QString& addressInput);
        const QString& addressInput;
        int& userNum);

};

#endif // USERMANAGEMENT_H
