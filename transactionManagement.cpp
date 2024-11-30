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
    BookManagement* bookManager = BookManagement::getBookManager();
    UserManagement* userManager = UserManagement::getUserManager();

    //getting current user
    QJsonObject currentUser = userManager->getCurrentUser();

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

bool TransactionManagement::removeHold(const QString& isbn, const QString& account){

    QJsonArray books = getBookArray();
    QJsonObject user = getUserObjAccount(account);

    //find book
    for(int i = 0; i<books.size(); i++){
        QJsonObject book = books[i].toObject();
        if(book["isbn"].toString() == isbn){
            QJsonArray inQueue = book["inQueue"].toArray();
            bool userFound = false;
            bool nextInLine = false;

            for(int j = 0; j<inQueue.size(); j++){
                QJsonObject queuedUser = inQueue[j].toObject();
                if(queuedUser["username"].toString() == user["username"].toString()){
                    //if J is 0 means user was next inQueue
                    if(j == 0){
                        nextInLine = true;
                    }
                    inQueue.removeAt(j);
                    userFound = true;
                    break;
                }
            }

            if(userFound){
                //notify next inQueue if user was next
                if(nextInLine && !inQueue.isEmpty()){
                    QJsonObject nextInQueue = inQueue.first().toObject();
                    nextInQueue["holdStatus"] = "ready";
                    inQueue.replace(0, nextInQueue);
                }

                book["inQueue"] = inQueue;

                //if queue empty set to available
                if(inQueue.isEmpty()){
                    book["isAvailable"] = true;
                }

                books.replace(i,book);

                //update database
                QJsonObject& database = getFileData();

                database["books"] = books;

                //save updated data to JSON file
                if(saveData()){
                    qDebug()<<"TransactionManagement: Hold removal successful";
                    return true;
                } else {
                    qDebug()<<"TransactionManagement: Faild to complete hold removal";
                    return false;
                }
            } else {
                qDebug()<<"TransactionManagement: User not in queue";
                return false;
            }
        }
    }

    qDebug()<<"TransactionManagement: Book not found";
    return false;
}

bool TransactionManagement::checkoutBook(const QString& isbn){
    //getting managers
    BookManagement* bookManager = BookManagement::getBookManager();
    UserManagement* userManager = UserManagement::getUserManager();

    //getting current user
    QJsonObject& user = userManager->getCurrentUser();

    if(user.isEmpty()){
        qDebug()<<"TransactionManagement: Current user is empty";
        return false;
    }

    //update book to unavailable
    QJsonArray books = bookManager->getBookArray();
    for(int i = 0; i<books.size(); ++i){
        QJsonObject book = books[i].toObject();
        if(book["isbn"].toString()==isbn){
            book["isAvailable"]=false;
            books.replace(i, book);
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
    QJsonArray users = userManager->getUserArray();
    for(int i = 0; i<users.size(); ++i){
        QJsonObject updatedUser = users[i].toObject();
        if(updatedUser["username"].toString() == user["username"].toString()){
            users.replace(i, user);
            break;
        }
    }

    QJsonObject& fileData = getFileData();

    //userManager->updateUserArray(users);
    //update and save database
    fileData["users"] = users;
    fileData["books"] = books;


    //updating current user
    userManager->updateCurrentUser();

    //save updated data to JSON file
    if(saveData()){
        qDebug()<<"TransactionManagement: Checkout Successfull";
        return true;
    }

    qDebug()<<"TransactionManagement: Checkout Failed";
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
