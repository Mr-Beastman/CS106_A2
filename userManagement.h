#ifndef USERMANAGEMENT_H
#define USERMANAGEMENT_H

#include "dataManagement.h"

class UserManagement : public DataManagement
{
public:
    UserManagement(const QString& dbPath);

    bool verifyLogin(const QString& usernameInput, const QString& passwordInput);
    bool isAdmin(const QString& usernameInput);
    bool isActive(const QString &usernameInput);
    bool addUser();
};

#endif // USERMANAGEMENT_H
