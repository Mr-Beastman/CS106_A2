#ifndef TRANSACTIONMANAGEMENT_H
#define TRANSACTIONMANAGEMENT_H

#include "userManagement.h"
#include "bookManagement.h"

class TransactionManagement : public UserManagement, public BookManagement {

public:
    TransactionManagement();

    //transaction methods
    bool placeHold(const QString isbn, const QString& username);
    bool removeHold(const QString& holdId);
    QString generateHoldId();
    QString checkHoldstatus(const QString& holdId);
    bool checkoutBook(const QString& isbn, const QString& username);
    bool returnBook(const QString& isbn, const QString &username);
    QString checkedOutTo(const QString& isbn);
    bool renewBook(const QString& username, const QString& isbn);
    QJsonArray getLoanHistory(const QString& username);
    QJsonArray getActiveLoans(const QString& username);
};

#endif // TRANSACTIONMANAGEMENT_H
