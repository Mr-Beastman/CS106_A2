#include "managementTransaction.h"

//place hold a curretnly checked out book
//parameters : username requesting hold and isbn of book to hold
//returns : true on success false on fail

managementTransaction::managementTransaction() : managementUser(), managementBook(){}

bool managementTransaction::placeHold(const QString isbn, const QString& username){
    //getting managers
    managementUser userManager;

    //getting current user
    QJsonObject& dataBase = getFileData();

    QString holdId = generateHoldId();

    //add user request to book data in json file
    QJsonArray bookArray = dataBase["books"].toArray();
    for(int i = 0; i<bookArray.size(); i++){
        QJsonObject book = bookArray[i].toObject();

        if(book["isbn"].toString() == isbn){
            QJsonArray bookQueue = book["inQueue"].toArray();
            QJsonObject holdEntry;

            //creating hold entry
            //storing username for queue
            holdEntry["username"] = username;
            //storing time of request to so oldest can be priortised
            holdEntry["holdDate"] = QDateTime::currentDateTime().date().toString("dd-mm-yyyy");
            //setting a status active/notified/expired for auto clearing
            holdEntry["holdStatus"] =  "active";
            //setting a hold id
            holdEntry["holdId"] = holdId;

            bookQueue.append(holdEntry);

            book["inQueue"] = bookQueue;

            bookArray.replace(i, book);
            break;
        }
    }

    QJsonArray userArray = dataBase["users"].toArray();
    //add book isbn to user account so we can check what books they have on hold
    for(int i = 0; i<userArray.size(); i++){
        QJsonObject user = userArray[i].toObject();
        if(user["username"].toString() == username){
            QJsonArray holdRequests = user["holdRequests"].toArray();
            QJsonObject holdEntry;

            holdEntry["isbn"] = isbn;
            holdEntry["holdId"] = holdId;
            holdRequests.append(holdEntry);

            user["holdRequests"] = holdRequests;

            userArray.replace(i,user);
            break;
        }
    }

    QJsonObject& fileData = getFileData();

    //update and save database
    fileData["users"] = userArray;
    fileData["books"] = bookArray;


    //save updated data to Json file
    if(saveData()){
        qDebug()<<"managementTransaction: Hold Successfull";
        return true;
    }

    qDebug()<<"managementTransaction: Hold Failed";
    return false;
}

bool managementTransaction::removeHold(const QString& holdId){

    QJsonArray books = getBookArray();

    //check which book contains holdId
    for(int i = 0; i<books.size(); i++){
        QJsonObject book = books[i].toObject();
        QJsonArray inQueue = book["inQueue"].toArray();

        bool userFound = false;
        bool nextInQueue = false;

        for(int j = 0; j<inQueue.size(); j++){
            QJsonObject queuedUser = inQueue[i].toObject();
            if(queuedUser["holdId"].toString() == holdId){
                if(j == 0){
                    nextInQueue = true;
                }

                inQueue.removeAt(j);
                userFound = true;
                break;
            }
        }

        if(userFound){
            if(nextInQueue && !inQueue.isEmpty()){
                QJsonObject nextInQueue = inQueue.first().toObject();
                nextInQueue["holdStatus"] = "ready";
                inQueue.replace(0, nextInQueue);
            }

            book["inQueue"] = inQueue;

            if(inQueue.isEmpty()){
                book["isAvailable"] = true;
            }

            books.replace(i, book);

            QJsonObject& database = getFileData();

            database["books"] = books;

            if(saveData()){
                qDebug()<<"managementTransaction: Hold removed successfully";
                return true;
            } else {
                qDebug()<<"managementTransaction: Failed to remove hold";
                return false;
            }
        }
    }

    qDebug()<<"managementTransaction: Hold Id not found";
    return false;
}


QString managementTransaction::generateHoldId() {
    managementUser userManager;

    QJsonArray userArray = getUserArray();

    QString newHoldId;
    bool isUnique = false;

    while (!isUnique) {

        newHoldId = QUuid::createUuid().toString();
        isUnique = true;

        for (const QJsonValue& userValue : userArray) {
            QJsonObject user = userValue.toObject();
            QJsonArray holdRequests = user["holdRequests"].toArray();

            for (const QJsonValue& holdValue : holdRequests) {
                QJsonObject hold = holdValue.toObject();

                if (hold["holdId"].toString() == newHoldId) {
                    isUnique = false;

                }
            }

            if (!isUnique) {
            }
        }
    }
    return newHoldId;
}

QString managementTransaction::checkHoldstatus(const QString &holdId){
    QJsonArray bookArray = getBookArray();

    for(int i = 0; i<bookArray.size(); i++){
        QJsonObject book = bookArray[i].toObject();

        QJsonArray inQueue = book["inQueue"].toArray();

        for(int j = 0; j<inQueue.size(); j++){
            QJsonObject entry = inQueue[i].toObject();

            if(entry["holdId"].toString() == holdId){
                qDebug()<<"managementTransaction: Returning hold Status";
                return entry["holdStatus"].toString();
            }
        }
    }
    //if not found display error and return empty string
    qDebug()<<"managementTransaction: Hold ID not found";
    return QString();

}



bool managementTransaction::checkoutBook(const QString& isbn, const QString& username) {
    // Getting managers
    managementBook bookManager;
    managementUser userManager;

    // Getting the current user
    QJsonObject user = userManager.getUserObj(username);

    if (user.isEmpty()) {
        qDebug() << "managementTransaction: Current user is empty";
        return false;
    }

    // Update book to unavailable
    QJsonArray books = bookManager.getBookArray();
    bool bookFound = false;
    for (int i = 0; i < books.size(); ++i) {
        QJsonObject book = books[i].toObject();
        if (book["isbn"].toString() == isbn) {
            book["isAvailable"] = false;  // Mark book as unavailable
            books.replace(i, book);  // Replace the updated book in the array
            bookFound = true;
            break;
        }
    }

    if (!bookFound) {
        qDebug() << "managementTransaction: Book not found";
        return false;
    }

    QJsonArray activeLoans = user["activeLoans"].toArray();

    for (const auto& loan : activeLoans) {
        if (loan.toObject()["isbn"].toString() == isbn) {
            qDebug() << "managementTransaction: This book is already checked out by the user.";
            return false;  // Prevent duplicate checkouts for the same book
        }
    }

    QJsonObject loanEntry;
    loanEntry["isbn"] = isbn;
    loanEntry["checkoutDate"] = QDateTime::currentDateTime().toString();
    loanEntry["dueDate"] = QDateTime::currentDateTime().addDays(30).toString();

    activeLoans.append(loanEntry);

    user["activeLoans"] = activeLoans;

    // Updating the user in the userArray
    QJsonArray users = getUserArray();
    for (int i = 0; i < users.size(); ++i) {
        QJsonObject updatedUser = users[i].toObject();
        if (updatedUser["username"].toString() == user["username"].toString()) {
            users.replace(i, user);
            break;
        }
    }

    // reference file data to update directly
    QJsonObject& fileData = getFileData();

    // Update and save the database
    fileData["users"] = users;
    fileData["books"] = books;

    // Updating the current user
//    userManager.updateCurrentUser();

    if (saveData()) {
        qDebug() << "managementTransaction: Checkout Successful";
        return true;
    }

    qDebug() << "managementTransaction: Checkout Failed";
    return false;
}

bool managementTransaction::returnBook(const QString &isbn, const QString& username) {
    // Getting managers
    managementBook bookManager;
    managementUser userManager;

    QJsonObject userObj = userManager.getUserObj(username);

    // Checking if the book is in the user's active loans
    QJsonArray activeLoans = userObj["activeLoans"].toArray();
    bool foundBook = false;
    for (int i = 0; i < activeLoans.size(); i++) {
        QJsonObject loan = activeLoans[i].toObject();
        if (loan["isbn"].toString() == isbn) {
            activeLoans.removeAt(i);
            userObj["activeLoans"] = activeLoans;
            foundBook = true;
            break;
        }
    }

    if (!foundBook) {
        qDebug() << "managementTransaction: Book not in user's active loans";
        return false;
    }

    // Changing book availability
    QJsonObject& database = getFileData();

    QJsonArray books = database["books"].toArray();
    for (int i = 0; i < books.size(); i++) {
        QJsonObject book = books[i].toObject();
        if (book["isbn"].toString() == isbn) {
            QJsonArray inQueue = book["inQueue"].toArray();

            if (!inQueue.isEmpty()) {
                // get first person in queue
                QJsonObject nextInQueue = inQueue.first().toObject();

                // Update hold status
                nextInQueue["holdStatus"] = "ready";
                inQueue.replace(0, nextInQueue);

                book["inQueue"] = inQueue;
            } else {
                // if no one in queue setting to available
                book["isAvailable"] = true;
            }

            // Update the book in the array
            books.replace(i, book);
            break;
        }
    }

    // Save updated data to the database

    QJsonArray users = database["users"].toArray();
    for (int i = 0; i < users.size(); i++) {
        QJsonObject user = users[i].toObject();
        if (user["username"].toString() == username) {
            users.replace(i, userObj);
            break;
        }
    }

    database["users"] = users;
    database["books"] = books;

    if (saveData()) {
        qDebug() << "managementTransaction: Book returned";
        return true;
    } else {
        qDebug() << "managementTransaction: Failed to return book";
        return false;
    }
}

QString managementTransaction::checkedOutTo(const QString &isbn){

    QJsonObject& database = getFileData();
    QJsonArray users = database["users"].toArray();

    for(int i = 0; i < users.size(); i++){
        QJsonObject user = users[i].toObject();
        QJsonArray activeLoans = user["activeLoans"].toArray();
        if(!activeLoans.isEmpty()){
            for(int j = 0; j < activeLoans.size(); j++){
                QJsonObject loan = activeLoans[j].toObject();

                if(loan["isbn"].toString() == isbn){
                    return user["username"].toString();
                }
            }
        }
    }

    //if no match found return none
    return "Not Currently Checked Out";
}
