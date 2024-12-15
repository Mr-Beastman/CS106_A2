#ifndef ManagementTransaction_H
#define ManagementTransaction_H

#include "managementUser.h"
#include "managementBook.h"

#include "qpushbutton.h"

class ManagementTransaction : public ManagementUser, public ManagementBook {

private:
    QJsonArray holdArray;

public:
    ManagementTransaction();

    //setters
    void setHoldArray();

    //getters
    QJsonArray getHoldArray();

    QJsonObject getHoldDetails(const QString& holdId);

    //transaction methods
    bool placeHold(const QString isbn, const QString& username);
    bool removeHold(const QString& holdId);
    bool notifyNextInQueue(const QString& isbn);
    QString generateHoldId();
    QString checkHoldstatus(const QString& holdId);
    bool checkoutBook(const QString& isbn, const QString& username);
    bool returnBook(const QString& isbn, const QString &username);
    QString checkedOutTo(const QString& isbn);
    bool bookIsDue(const QString &username, const QString &isbn);
    bool bookIsOverDue(const QString &username, const QString &isbn);
    void setBookAvailibityOptions(QWidget* uiObject, const QJsonObject& book, const QString& username);
};

#endif // ManagementTransaction_H
