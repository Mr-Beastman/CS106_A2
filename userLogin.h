#ifndef USERLOGIN_H
#define USERLOGIN_H


#include <QtSql>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QLineEdit>

class userVerification{

    QSqlDatabase userDatabase= QSqlDatabase::addDatabase("QSQLITE");

    QLineEdit* username;
    QLineEdit* password;

public:
    userVerification();

    //setters
    void setUsername(QLineEdit* usernameInput);
    void setPassword(QLineEdit* passwordInput);

    bool userLogin();
    bool isAdmin();
    bool isActive();

};

#endif // USERLOGIN_H
