#ifndef USERMANAGEMENT_H
#define USERMANAGEMENT_H

#include "dataManagement.h"

class UserManagement : public DataManagement {

private:
    UserManagement();
    ~UserManagement();

    //setting up singtleton
    //static ptr to bookmanager instance
    static UserManagement* userManager;
    static QMutex userMtx;

protected:
    QJsonObject currentUser;

public:
    //enforcing singleton
    //removing copy contructor to prevent copies
    UserManagement(const UserManagement& obj) = delete;
    //removing assigment operator to prevent assignment
    UserManagement& operator=(const UserManagement& obj) = delete;

    //setters
    void setCurrentUser(const QString usernameInput);

    //getters
    QJsonObject getCurrentUser();
    QJsonObject getUser(const QString &username);

    //get the bookmanager instance
    static UserManagement* getUserManager();

    int userID();
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

};

#endif // USERMANAGEMENT_H
