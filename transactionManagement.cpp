#include "transactionManagement.h"

TransactionManagement::TransactionManagement() : UserManagement(), BookManagement() {}

//intializing static memebers
TransactionManagement * TransactionManagement:: transactionManager = nullptr;
QMutex TransactionManagement::transactionMtx;

//getter for singleton
TransactionManagement* TransactionManagement::getTransactionManager(){
    if(transactionManager == nullptr){
        QMutexLocker locker(&transactionMtx);
        if(transactionManager == nullptr){
            transactionManager = new TransactionManagement();
        }
    }
    return transactionManager;
}

//place hold a curretnly checked out book
//parameters : username requesting hold and isbn of book to hold
//returns : true on success false on fail
bool TransactionManagement::placeHold(const QString &username, const QString &isbn){

    //add user request to book data in json file
    for(int i = 0; bookArray.size(); i++){
        QJsonObject book = bookArray[i].toObject();

        if(book["isbn"].toString() == isbn){
            QJsonArray bookQueue = book["inQueue"].toArray();

            QJsonObject holdEntry;

            //storing username for queue
            holdEntry["username"] = username;
            //storing time of request to so oldest can be priortised
            holdEntry["holdDate"] = QDateTime::currentDateTime().toString();
            //setting a stats active/notified/expired for auto clearing
            holdEntry["holdStatus"] =  "active";

            bookQueue.append(holdEntry);

            book["inQueue"] = bookQueue;

            bookArray[i] = book;
            break;
        }
    }

    //add book isbn to user account so we can check what books they have on hold
    for(int i = 0; userArray.size(); i++){
        QJsonObject user = userArray[i].toObject();
        if(user["username"].toString() == username){
            QJsonArray holdRequests = user["holdRequests"].toArray();
            QJsonObject holdEntry;

            holdEntry["isbn"] = isbn;
            holdRequests.append(holdEntry);

            user["holdRequests"] = holdRequests;

            userArray[i] = user;
            break;
        }
    }

    //update and save database
    libraryDatabase["users"] = userArray;
    libraryDatabase["books"] = bookArray;

    if(saveData()){
        qDebug()<<"TransactionManagement: Hold Successfull";
        return true;
    }

    qDebug()<<"TransactionManagement: Hold Failed";
    return false;
}

bool TransactionManagement::checkoutBook(const QString& isbn){
    //getting managers
    BookManagement* bookManager = BookManagement::getBookManager();
    UserManagement* userManager = UserManagement::getUserManager();

    //getting current user
    QJsonObject user = userManager->getCurrentUser();

    //update book to unavailable
    QJsonArray bookArray = bookManager->getBookArray();

    //setting the book to unavailable
    for(int i = 0; i < bookArray.size(); ++i){
        QJsonObject book = bookArray[i].toObject();
        if(book["isbn"].toString()==isbn){
            book["isAvailable"]=false;
            bookArray.replace(i, book);
            break;
        }
    }

    //adding to book users active loans
    QJsonArray activeLoans = user["activeLoans"].toArray();
    QJsonObject loanEntry;

    loanEntry["isbn"] = isbn;
    loanEntry["checkoutDate"] = QDateTime::currentDateTime().toString();
    loanEntry["dueDate"] =  QDateTime::currentDateTime().addDays(30).toString();
    activeLoans.append(loanEntry);

    user["activeLoans"] = activeLoans;

    //updating user in the userArray
    QJsonArray userArray = userManager->getUserArray();
    for(int i = 0; i<userArray.size(); ++i){
        QJsonObject updatedUser = userArray[i].toObject();
        if(updatedUser["username"].toString() == user["username"].toString()){
            userArray.replace(i, user);
            break;
        }
    }

    //update the stored library database.
    libraryDatabase["users"] = userArray;
    libraryDatabase["books"] = bookArray;

    //updating current user
    userManager->setCurrentUser(user);

    //save updated data to JSON file
    if(saveData()){
        qDebug()<<"TransactionManagement: Checkout Successfull";
        return true;
    }

    qDebug()<<"TransactionManagement: Checkout Failed";
    return false;
}
