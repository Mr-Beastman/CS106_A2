#ifndef USERMANAGEMENT_H
#define USERMANAGEMENT_H

#include "dataManagement.h"

class UserManagement : public DataManagement
{
public:
    UserManagement(const QString& dbPath);

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
        const QString& addressInput,
        int& userNum);
};

#endif // USERMANAGEMENT_H
