#include "userLogin.h"
#include "utilities.h"


userVerification::userVerification() {}

void userVerification::setUsername(QLineEdit *usernameInput)
{
    username=usernameInput;
}

void userVerification::setPassword(QLineEdit *passwordInput)
{
    password=passwordInput;
}

bool userVerification::userLogin() {

    //setting database location
    userDatabase.setDatabaseName(utilities::setDatabasePath());

    //open database for checking
    userDatabase.open();

    QSqlQuery checkLogin(userDatabase);
    checkLogin.prepare("SELECT * FROM logins WHERE username = :username AND password = :password");
    checkLogin.bindValue(":username", username->text());
    checkLogin.bindValue(":password", password->text());

    if (checkLogin.exec()){
        if(checkLogin.next()){
            userDatabase.close();
            return 1;
        }
    }

    userDatabase.close();
    return 0;
}

bool userVerification::isAdmin() {

    //setting database location
    userDatabase.setDatabaseName(utilities::setDatabasePath());

    //open database for checking
    userDatabase.open();

    QSqlQuery checkLogin(userDatabase);
    checkLogin.prepare("SELECT isAdmin FROM logins WHERE username = :username");
    checkLogin.bindValue(":username", username->text());

    if (checkLogin.exec()){
        if(checkLogin.next()){
            userDatabase.close();
            return checkLogin.value(0).toInt()==1;
        }
    }

    userDatabase.close();
    return 0;
}

bool userVerification::isActive()
{
    //setting database location
    userDatabase.setDatabaseName(utilities::setDatabasePath());

    //open database for checking
    userDatabase.open();

    QSqlQuery checkLogin(userDatabase);
    checkLogin.prepare("SELECT activated FROM logins WHERE username = :username");
    checkLogin.bindValue(":username", username->text());

    if (checkLogin.exec()){
        if(checkLogin.next()){
            userDatabase.close();
            return checkLogin.value(0).toInt()==1;
        }
    }

    userDatabase.close();
    return 0;
}
