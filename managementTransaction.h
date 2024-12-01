#ifndef MANAGEMENTTRANSACTION_H
#define MANAGEMENTTRANSACTION_H

#include "managementUser.h"
#include "managementBook.h"

class managementTransaction : public managementUser, public managementBook {

public:
    managementTransaction();

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

#endif // MANAGEMENTTRANSACTION_H
