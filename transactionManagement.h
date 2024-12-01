#ifndef TRANSACTIONMANAGEMENT_H
#define TRANSACTIONMANAGEMENT_H

#include "userManagement.h"
#include "bookManagement.h"

class TransactionManagement : public UserManagement, public BookManagement {

private:
    //singleton
    static TransactionManagement* transactionManager;
    static QMutex transactionMtx;

    //private constructor
    TransactionManagement();

public:
    //enforcing singleton
    TransactionManagement(const TransactionManagement& obj) = delete;
    TransactionManagement& operator=(const TransactionManagement& obj) = delete;

    //getter for transactionManager
    static TransactionManagement* getTransactionManager();

    //transaction methods
    bool placeHold(const QString& isbn);
    bool removeHold(const QString& holdId);
    QString generateHoldId();
    QString checkHoldstatus(const QString& holdId);
    bool checkoutBook(const QString& isbn);
    bool returnBook(const QString& isbn);
    QString checkedOutTo(const QString& isbn);
    bool renewBook(const QString& username, const QString& isbn);
    QJsonArray getLoanHistory(const QString& username);
    QJsonArray getActiveLoans(const QString& username);
};

#endif // TRANSACTIONMANAGEMENT_H
