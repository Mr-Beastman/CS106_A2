#include "managementTransaction.h"

//place hold a curretnly checked out book
//parameters : username requesting hold and isbn of book to hold
//returns : true on success false on fail

ManagementTransaction::ManagementTransaction() : ManagementUser(), ManagementBook(){
    setHoldArray();
}

void ManagementTransaction::setHoldArray(){
    QJsonObject& database = getFileData();
    holdArray = database["holds"].toArray();
}

QJsonArray ManagementTransaction::getHoldArray(){
    return holdArray;
}

QJsonObject ManagementTransaction::getHoldDetails(const QString &holdId){

    for(int i = 0; i<holdArray.size(); i++){
        QJsonObject hold = holdArray[i].toObject();

        if(hold["holdId"].toString() == holdId){
            return hold;
        }
    }

    qDebug()<<"ManagementTransaction: Hold ID cannot be found";
    return QJsonObject();
}

bool ManagementTransaction::placeHold(const QString isbn, const QString& username){
    //getting managers
    ManagementUser userManager;

    //getting current user
    QJsonObject& dataBase = getFileData();

    //generate unique holdId
    QString holdId = generateHoldId();



    //add holdId to book
    QJsonArray bookArray = dataBase["books"].toArray();
    for(int i = 0; i<bookArray.size(); i++){
        QJsonObject book = bookArray[i].toObject();

        if(book["isbn"].toString() == isbn){
            QJsonArray bookQueue = book["inQueue"].toArray();
            QJsonObject holdEntry;

            holdEntry["holdId"] = holdId;

            bookQueue.append(holdEntry);

            book["inQueue"] = bookQueue;

            bookArray.replace(i, book);
            break;
        }
    }

    //add holdId to user
    QJsonArray userArray = dataBase["users"].toArray();
    //add book isbn to user account so we can check what books they have on hold
    for(int i = 0; i<userArray.size(); i++){
        QJsonObject user = userArray[i].toObject();
        if(user["username"].toString() == username){
            QJsonArray holdRequests = user["holdRequests"].toArray();
            QJsonObject holdEntry;
            holdEntry["holdId"] = holdId;
            holdRequests.append(holdEntry);
            user["holdRequests"] = holdRequests;
            userArray.replace(i,user);
            break;
        }
    }

    //obj to store hold information
    QJsonObject holdRequest;

    //add hold record to array
    holdRequest["holdId"] = holdId;
    holdRequest["username"] = username;
    holdRequest["isbn"] = isbn;
    holdRequest["holdDate"] = QDateTime::currentDateTime().date().toString("dd-mm-yyyy");
    holdRequest["holdStatus"] =  "active";
    holdArray.append(holdRequest);

    QJsonObject& fileData = getFileData();

    //update and save database
    fileData["users"] = userArray;
    fileData["books"] = bookArray;
    fileData["holds"] = holdArray;


    //save updated data to Json file
    if(saveData()){
        qDebug()<<"ManagementTransaction: Hold Successfull";
        return true;
    }

    qDebug()<<"ManagementTransaction: Hold Failed";
    return false;
}

//remove hold request. removes hold Id from book, user and hold database;
//paramaters: holdId for hold to be removed
//returns: true on success. false on failure
bool ManagementTransaction::removeHold(const QString& holdId){

    qDebug()<<"ManagementTransaction: Removind hold "<<holdId;
    QJsonObject hold = getHoldDetails(holdId);

    if(hold.isEmpty()){
        qDebug()<<"ManagementTransaction: Failed to retrieve hold details";
        return false;
    }

    //removing hold id from book
    for(int i = 0; i<bookArray.size(); i++){
        QJsonObject book = bookArray[i].toObject();
        QJsonArray inQueue = book["inQueue"].toArray();

        for(int j = 0; j<inQueue.count(); j++){
            QJsonObject queueEntry = inQueue[j].toObject();
            if(queueEntry["holdId"].toString() == holdId){
                inQueue.removeAt(j);
                book["inQueue"] = inQueue;
                bookArray.replace(i, book);
                qDebug()<<"ManagementTransaction: Hold id removed from book";
                break;
            }
        }
    }

    //removing hold id from user
    for(int i = 0; i<userArray.size(); i++){
        QJsonObject user = userArray[i].toObject();
        QJsonArray userHolds = user["holdRequests"].toArray();

        for(int j = 0; j<userHolds.count(); j++){
            QJsonObject queueEntry = userHolds[j].toObject();
            if(queueEntry["holdId"].toString() == holdId){
                userHolds.removeAt(j);
                user["holdRequests"] = userHolds;
                userArray.replace(i, user);
                qDebug()<<"ManagementTransaction: Hold id removed from user";
                break;
            }
        }
    }

     //removing hold id from holds
    for(int i = 0; i<holdArray.size(); i++){
        QJsonObject hold = holdArray[i].toObject();
        if(hold["holdId"].toString() == holdId){
            holdArray.removeAt(i);
            qDebug()<<"ManagementTransaction: Hold id removed from holds";
            break;
        }
    }

    //update database for saving
    QJsonObject& database = getFileData();

    database["books"]=bookArray;
    database["users"]=userArray;
    database["holds"]=holdArray;

    //save updated data
    if(saveData()){
        qDebug()<<"ManagementTransaction: Hold removed successfully";
        return true;
    } else {
        qDebug()<<"ManagementTransaction: Failed to save updated hold data";
        return false;
    }
}

bool ManagementTransaction::notifyNextInQueue(const QString &isbn){

    QString holdId;

    for(int i = 0; i<bookArray.size(); i++){
        QJsonObject book = bookArray[i].toObject();

        if(book["isbn"].toString() == isbn){
            QJsonArray inQueue = book["inQueue"].toArray();
            if(!inQueue.isEmpty()){
                QJsonObject hold = inQueue[0].toObject();
                holdId = hold["holdId"].toString();
                qDebug()<<"ManagementTransaction: Next in Queue Hold Id : "<<holdId;

            } else {
                qDebug()<<"ManagementTransaction: Book has no next in queue";
                return false;
            }
        }
    }

    for(int i = 0; i<holdArray.size(); i ++){
        QJsonObject hold = holdArray[i].toObject();
        if(hold["holdId"].toString() == holdId && hold["holdStatus"].toString() == "active"){
            hold["holdStatus"] = "ready";
            holdArray.replace(0, hold);
            break;
        } else {
            qDebug()<<"ManagementTransaction: Hold Id not found or invalid status";
            return false;
        }
    }

    //update database for saving
    QJsonObject& database = getFileData();

    database["holds"]=holdArray;

    //save updated data
    if(saveData()){
        qDebug()<<"ManagementTransaction: Hold successfully set to ready";
        return true;
    } else {
        qDebug()<<"ManagementTransaction: Failed to save update hold data";
        return false;
    }

}


QString ManagementTransaction::generateHoldId() {
    ManagementUser userManager;

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

QString ManagementTransaction::checkHoldstatus(const QString &holdId){
    QJsonArray bookArray = getBookArray();

    for(int i = 0; i<bookArray.size(); i++){
        QJsonObject book = bookArray[i].toObject();

        QJsonArray inQueue = book["inQueue"].toArray();

        for(int j = 0; j<inQueue.size(); j++){
            QJsonObject entry = inQueue[i].toObject();

            if(entry["holdId"].toString() == holdId){
                qDebug()<<"ManagementTransaction: Returning hold Status";
                return entry["holdStatus"].toString();
            }
        }
    }
    //if not found display error and return empty string
    qDebug()<<"ManagementTransaction: Hold ID not found";
    return QString();

}



bool ManagementTransaction::checkoutBook(const QString& isbn, const QString& username) {
    // Getting managers

    // Getting the current user
    QJsonObject user = getUserObj(username);

    qDebug()<<"ManagementTransaction: Checking out "<<isbn<<" to "<<user["username"].toString();

    if (user.isEmpty()) {
        qDebug() << "ManagementTransaction: Current user is empty";
        return false;
    }

    qDebug()<<"ManagementTransaction: Setting book to issued details";
    // Update book to unavailable
    for (int i = 0; i < bookArray.size(); ++i) {
        QJsonObject book = bookArray[i].toObject();
        if (book["isbn"].toString() == isbn) {
            book["isAvailable"] = false;
            book["issuedTo"] = username;
            bookArray.replace(i, book);
            break;
        }
    }

    qDebug()<<"ManagementTransaction: Setting user to issued details";

    QJsonArray activeLoans = user["activeLoans"].toArray();

    QJsonObject loanEntry;
    loanEntry["isbn"] = isbn;
    loanEntry["checkoutDate"] = QDateTime::currentDateTime().toString();
    loanEntry["dueDate"] = QDateTime::currentDateTime().addDays(30).toString("dd/MM/yyyy");
    loanEntry["status"] = "active";

    activeLoans.append(loanEntry);

    user["activeLoans"] = activeLoans;

    // Updating the user in the userArray
    for (int i = 0; i < userArray.size(); ++i) {
        QJsonObject updatedUser = userArray[i].toObject();
        if (updatedUser["username"].toString() == user["username"].toString()) {
            userArray.replace(i, user);
            break;
        }
    }

    // reference file data to update directly
    QJsonObject& fileData = getFileData();

    // Update and save the database
    fileData["users"] = userArray;
    fileData["books"] = bookArray;

    if (saveData()) {
        qDebug() << "ManagementTransaction: Checkout Successful";
        return true;
    }

    qDebug() << "ManagementTransaction: Checkout Failed";
    return false;
}

bool ManagementTransaction::returnBook(const QString &isbn, const QString& username) {
    // Getting managers
    ManagementBook bookManager;
    ManagementUser userManager;

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
        qDebug() << "ManagementTransaction: Book not in user's active loans";
        return false;
    }

    // Changing book availability


    for (int i = 0; i < bookArray.size(); i++) {
        QJsonObject book = bookArray[i].toObject();

        if (book["isbn"].toString() == isbn) {
            QJsonArray inQueue = book["inQueue"].toArray();

            book["issuedTo"] = QString();

            if (inQueue.isEmpty()) {
                // if no one in queue setting to available
                book["isAvailable"] = true;
            }

            // Update the book in the array
            bookArray.replace(i, book);
            break;
        }
    }

    // Save updated data to the database

    for (int i = 0; i < userArray.size(); i++) {
        QJsonObject user = userArray[i].toObject();
        if (user["username"].toString() == username) {
            userArray.replace(i, userObj);
            break;
        }
    }

    QJsonObject& database = getFileData();

    database["users"] = userArray;
    database["books"] = bookArray;

    if (saveData()) {
        qDebug() << "ManagementTransaction: Book returned";
        return true;
    } else {
        qDebug() << "ManagementTransaction: Failed to return book";
        return false;
    }
}

QString ManagementTransaction::checkedOutTo(const QString &isbn){

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

void ManagementTransaction::setBookAvailibityOptions(ViewBookItem *viewBookItem, const QJsonObject &book, const QString& username){
    QStackedWidget* availabilityWidget = viewBookItem->findChild<QStackedWidget*>("availabilityWidget");
    QStackedWidget* optionsWidget = viewBookItem->findChild<QStackedWidget*>("optionsStackedWidget");
    QWidget* availabilityPage = nullptr;
    QWidget* optionsPage = nullptr;
    int index = 0;

    if (book["isAvailable"].toBool()) {
        //book is available
        availabilityPage = viewBookItem->findChild<QWidget*>("availablePage");
        optionsPage = viewBookItem->findChild<QWidget*>("userAvailablePage");
    } else if (book["issuedTo"].toString() == username) {
        //book is checked out by the current user
        availabilityPage = viewBookItem->findChild<QWidget*>("checkedoutPage");
        optionsPage = viewBookItem->findChild<QWidget*>("userCheckedoutPage");
        //setting due back date
        QLabel* dueLabel = viewBookItem->findChild<QLabel*>("dueOutputLabel");
        dueLabel->setText(getDueDate(username, book["isbn"].toString()));
    } else {
        //book is unavailable, checking if the user has a hold
        bool userHold = false;
        QString status;
        QString holdId;

        for (int j = 0; j < holdArray.size(); ++j) {
            QJsonObject hold = holdArray[j].toObject();
            if (hold["isbn"].toString() == book["isbn"].toString() && hold["username"].toString() == username) {
                userHold = true;
                holdId = hold["holdId"].toString();
                status = hold["holdStatus"].toString();
                break;
            }
        }

        if (userHold) {
            //storing hold ID
            QLabel* holdIdLabel = viewBookItem->findChild<QLabel*>("holdStoredIdLabel");
            holdIdLabel->setText(holdId);

            if (status == "ready") {
                availabilityPage = viewBookItem->findChild<QWidget*>("holdReadyDisplayPage");
                optionsPage = viewBookItem->findChild<QWidget*>("holdReadyPage");
            } else {
                availabilityPage = viewBookItem->findChild<QWidget*>("holdPendingPage");
                optionsPage = viewBookItem->findChild<QWidget*>("holdActivePage");
            }
        } else {
            //allow user to request hold
            availabilityPage = viewBookItem->findChild<QWidget*>("notAvailablePage");
            optionsPage = viewBookItem->findChild<QWidget*>("userNotAvailablePage");
        }
    }

    if (availabilityPage) {
        index = availabilityWidget->indexOf(availabilityPage);
        availabilityWidget->setCurrentIndex(index);
    }
    if (optionsPage) {
        index = optionsWidget->indexOf(optionsPage);
        optionsWidget->setCurrentIndex(index);
    }
}
