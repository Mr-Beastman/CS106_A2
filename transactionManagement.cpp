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
bool TransactionManagement::placeHold(const QString &isbn){
    //getting managers
    UserManagement* userManager = UserManagement::getUserManager();

    //getting current user
    QJsonObject currentUser = userManager->getCurrentUser();

    QString holdId = generateHoldId();

    //add user request to book data in json file
    QJsonArray bookArray = getBookArray();
    for(int i = 0; i<bookArray.size(); i++){
        QJsonObject book = bookArray[i].toObject();



        if(book["isbn"].toString() == isbn){
            QJsonArray bookQueue = book["inQueue"].toArray();
            QJsonObject holdEntry;

            //creating hold entry
            //storing username for queue
            holdEntry["username"] = currentUser["username"].toString();
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

    QJsonArray userArray = userManager->getUserArray();
    //add book isbn to user account so we can check what books they have on hold
    for(int i = 0; i<userArray.size(); i++){
        QJsonObject user = userArray[i].toObject();
        if(user["username"].toString() == currentUser["username"].toString()){
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

    //updating current user
    userManager->setCurrentUser(currentUser);

    //save updated data to Json file
    if(saveData()){
        qDebug()<<"TransactionManagement: Hold Successfull";
        return true;
    }

    qDebug()<<"TransactionManagement: Hold Failed";
    return false;
}

bool TransactionManagement::removeHold(const QString& holdId){

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
                qDebug()<<"TransactionManagement: Hold removed successfully";
                return true;
            } else {
                qDebug()<<"TransactionManagement: Failed to remove hold";
                return false;
            }
        }
    }

    qDebug()<<"TransactionManagement: Hold Id not found";
}


QString TransactionManagement::generateHoldId() {
    UserManagement* userManager = UserManagement::getUserManager();
    QJsonArray userArray = userManager->getUserArray();

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

QString TransactionManagement::checkHoldstatus(const QString &holdId){
    QJsonArray bookArray = getBookArray();

    for(int i = 0; i<bookArray.size(); i++){
        QJsonObject book = bookArray[i].toObject();

        QJsonArray inQueue = book["inQueue"].toArray();

        for(int j = 0; j<inQueue.size(); j++){
            QJsonObject entry = inQueue[i].toObject();

            if(entry["holdId"].toString() == holdId){
                qDebug()<<"TransactionManagement: Returning hold Status";
                return entry["holdStatus"].toString();
            }
        }
    }
    //if not found display error and return empty string
    qDebug()<<"TransactionManagement: Hold ID not found";
    return QString();

}



bool TransactionManagement::checkoutBook(const QString& isbn) {
    // Getting managers
    BookManagement* bookManager = BookManagement::getBookManager();
    UserManagement* userManager = UserManagement::getUserManager();

    // Getting the current user
    QJsonObject& user = userManager->getCurrentUser();

    if (user.isEmpty()) {
        qDebug() << "TransactionManagement: Current user is empty";
        return false;
    }

    // Update book to unavailable
    QJsonArray books = bookManager->getBookArray();
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
        qDebug() << "TransactionManagement: Book not found";
        return false;
    }

    QJsonArray activeLoans = user["activeLoans"].toArray();

    for (const auto& loan : activeLoans) {
        if (loan.toObject()["isbn"].toString() == isbn) {
            qDebug() << "TransactionManagement: This book is already checked out by the user.";
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
    QJsonArray users = userManager->getUserArray();
    for (int i = 0; i < users.size(); ++i) {
        QJsonObject updatedUser = users[i].toObject();
        if (updatedUser["username"].toString() == user["username"].toString()) {
            users.replace(i, user);
            break;
        }
    }

    // Get the file data
    QJsonObject& fileData = getFileData();

    // Update and save the database
    fileData["users"] = users;
    fileData["books"] = books;

    // Updating the current user
    userManager->updateCurrentUser();

    if (saveData()) {
        qDebug() << "TransactionManagement: Checkout Successful";
        return true;
    }

    qDebug() << "TransactionManagement: Checkout Failed";
    return false;
}

bool TransactionManagement::returnBook(const QString &isbn) {
    // Getting managers
    BookManagement* bookManager = BookManagement::getBookManager();
    UserManagement* userManager = UserManagement::getUserManager();

    QString username = checkedOutTo(isbn);
    QJsonObject userObj = userManager->getUserObj(username);

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
        qDebug() << "TransactionManagement: Book not in user's active loans";
        return false;
    }

    // Changing book availability
    QJsonArray books = bookManager->getBookArray();
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
    QJsonObject& database = getFileData();
    QJsonArray users = userManager->getUserArray();
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
        qDebug() << "TransactionManagement: Book returned";
        return true;
    } else {
        qDebug() << "TransactionManagement: Failed to return book";
        return false;
    }
}

QString TransactionManagement::checkedOutTo(const QString &isbn){

    qDebug()<<"Checking "<<isbn<<" for checkout";
    //looping through users

    QJsonArray users = getUserArray();

    qDebug()<<users;

    for(int i = 0; i < users.size(); i++){
        QJsonObject user = users[i].toObject();
        qDebug()<<"Checking "<<user["username"].toString();
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
