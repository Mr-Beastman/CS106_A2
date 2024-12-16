#include "managementTransaction.h"
#include <QPushButton>

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

        for(int j = 0; j<inQueue.size(); j++){
            QJsonObject queueEntry = inQueue[j].toObject();
            if(queueEntry["holdId"].toString() == holdId){
                inQueue.removeAt(j);
                book["inQueue"] = inQueue;
                //resetting book to availble if no further holds.
                if(inQueue.isEmpty() && book["issuedTo"].toString().isEmpty()){
                    book["isAvailable"] = true;
                }

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

    for(int i = 0; i<holdArray.size(); i++){
        QJsonObject hold = holdArray[i].toObject();

        qDebug()<<hold["holdId"].toString();
        if(hold["holdId"].toString() == holdId){
            return hold["holdStatus"].toString();
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

bool ManagementTransaction::bookIsDue(const QString& username, const QString& isbn){
    QJsonObject user = getUserObj(username);
    QJsonArray activeLoans = user["activeLoans"].toArray();

    for(int i = 0; i<activeLoans.size(); i++){
        QJsonObject loan = activeLoans[i].toObject();

        if(loan["isbn"].toString() == isbn){
            QDate dueDate = QDate::fromString(loan["dueDate"].toString(), "dd/MM/yyyy");
            QDate sevenDaysBefore = dueDate.addDays(-7);

            if(QDate::currentDate() >= sevenDaysBefore && QDate::currentDate() <= dueDate){
                return true;
            } else {
                return false;
            }
        }
    }
    qDebug()<<"Management Transaction: Due date could not be confirmed";
    return false;
}

bool ManagementTransaction::bookIsOverDue(const QString &username, const QString &isbn){
    QJsonObject user = getUserObj(username);
    QJsonArray activeLoans = user["activeLoans"].toArray();

    for(int i = 0; i<activeLoans.size(); i++){
        QJsonObject loan = activeLoans[i].toObject();

        if(loan["isbn"].toString() == isbn){
            QDate dueDate = QDate::fromString(loan["dueDate"].toString(), "dd/MM/yyyy");

            if(QDate::currentDate() > dueDate){
                return true;
            } else {
                return false;
            }
        }
    }
    qDebug()<<"Management Transaction: Due date could not be confirmed";
    return false;
}

void ManagementTransaction::setBookAvailibityOptions(QWidget* uiObject, const QJsonObject &book, const QString& username){
    QStackedWidget* availabilityWidget = uiObject->findChild<QStackedWidget*>("availabilityWidget");
    QStackedWidget* optionsWidget = uiObject->findChild<QStackedWidget*>("optionsStackedWidget");
    QWidget* availabilityPage = nullptr;
    QWidget* optionsPage = nullptr;
    int index = 0;

    if(isAdmin(username)){
        //setting admin displays
        index = optionsWidget->indexOf(uiObject->findChild<QWidget*>("adminPage"));
        optionsWidget->setCurrentIndex(index);

        //getting current book activity.
        QLabel* activeLoan  =  uiObject->findChild<QLabel*>("checkedOutputLabel");
        activeLoan->setText(checkedOutTo(book["isbn"].toString()));

        QJsonArray queue = book["inQueue"].toArray();

        QLabel* holdArray =  uiObject->findChild<QLabel*>("QueueOutputLabel");
        if(queue.isEmpty()){
            holdArray->setText(QString::number(book["inQueue"].toArray().size()));
        } else {
            holdArray->setText(QString::number(0));
        }

        //enabling/disabling return button depending on status
        QPushButton* returnButton = uiObject->findChild<QPushButton*>("returnButton");
        availabilityPage = uiObject->findChild<QStackedWidget*>("availabilityWidget");

        if(book["isAvailable"].toBool()){
            returnButton->hide();
            availabilityPage = uiObject->findChild<QWidget*>("availablePage");
        } else {
            if(!queue.isEmpty()){
                qDebug()<<"Hold status for "<<book["title"].toString()<<" is has a queue";
                QJsonObject firstInQueue = queue[0].toObject();
                QString holdStatus = checkHoldstatus(firstInQueue["holdId"].toString());
                qDebug()<<"Hold status for "<<book["title"].toString()<<" is "<< holdStatus;

                if(holdStatus == "ready"){
                    returnButton->hide();
                    availabilityPage = uiObject->findChild<QWidget*>("holdReadyDisplayPage");
                } else if (holdStatus == "active"){
                    returnButton->show();
                    availabilityPage = uiObject->findChild<QWidget*>("notAvailablePage");
                }
            } else {
                returnButton->show();
                availabilityPage = uiObject->findChild<QWidget*>("notAvailablePage");
            }
        }
        QLabel* queueLabel = uiObject->findChild<QLabel*>("QueueOutputLabel");
        queueLabel->setText(QString::number(queue.size()));

    } else {
        //setting members displays
        if (book["isAvailable"].toBool()) {
            //book is available
            availabilityPage = uiObject->findChild<QWidget*>("availablePage");
            optionsPage = uiObject->findChild<QWidget*>("userAvailablePage");
        } else if (book["issuedTo"].toString() == username) {
            //book is checked out by the current user
            if(bookIsDue(username, book["isbn"].toString())){
                availabilityPage = uiObject->findChild<QWidget*>("dueBackPage");
            } else if(bookIsOverDue(username, book["isbn"].toString())) {
                availabilityPage = uiObject->findChild<QWidget*>("overduePage");
            } else {
                availabilityPage = uiObject->findChild<QWidget*>("checkedoutPage");
            }
            optionsPage = uiObject->findChild<QWidget*>("userCheckedoutPage");
            //setting due date
            QLabel* dueLabel = uiObject->findChild<QLabel*>("dueOutputLabel");
            dueLabel->setText(getDueDate(username, book["isbn"].toString()));
        } else {
            //book is unavailable, checking if the user has a hold
            bool userHold = false;
            QString status;
            QString holdId;
            int place = 0;

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
                //getting place number
                QJsonArray holds = book["inQueue"].toArray();
                for(int k = 0; k<holds.size(); k++){
                    QJsonObject hold = holds[k].toObject();

                    if(hold["holdId"].toString()== holdId){
                        place = k+1;
                        break;
                    }
                }

                //storing hold ID
                QLabel* holdIdLabel = uiObject->findChild<QLabel*>("holdStoredIdLabel");
                holdIdLabel->setText(holdId);

                if (status == "ready") {
                    availabilityPage = uiObject->findChild<QWidget*>("holdReadyDisplayPage");
                    optionsPage = uiObject->findChild<QWidget*>("holdReadyPage");
                } else {
                    availabilityPage = uiObject->findChild<QWidget*>("holdPendingPage");
                    optionsPage = uiObject->findChild<QWidget*>("holdActivePage");
                    QLabel* dueLabel = uiObject->findChild<QLabel*>("queuePlaceOutPutLabel");
                    dueLabel->setNum(place);
                }
            } else {
                //allow user to request hold
                availabilityPage = uiObject->findChild<QWidget*>("notAvailablePage");
                optionsPage = uiObject->findChild<QWidget*>("userNotAvailablePage");
            }
        }
    }

    //hide hold request details used in admin book info view
    QLabel* requestLabel = uiObject->findChild<QLabel*>("requestLabel");
    QLabel* requestDisplayLabel = uiObject->findChild<QLabel*>("requestOutputLabel");

    if(requestLabel && requestDisplayLabel){
        requestLabel->hide();
        requestDisplayLabel->hide();
    }

    // requestLabel->hide();
    // requestDisplayLabel->hide();

    if (availabilityPage) {
        index = availabilityWidget->indexOf(availabilityPage);
        availabilityWidget->setCurrentIndex(index);
    }
    if (optionsPage) {
        index = optionsWidget->indexOf(optionsPage);
        optionsWidget->setCurrentIndex(index);
    }
}

void ManagementTransaction::updateOverdueLoans() {
    QDate currentDate = QDate::currentDate();
    bool updateMade = false;

    for (int i = 0; i < userArray.size(); i++) {
        QJsonObject user = userArray[i].toObject();

        if (user.contains("activeLoans")) {
            QJsonArray activeLoans = user["activeLoans"].toArray();

            for (int j = 0; j < activeLoans.size(); j++) {
                QJsonObject loan = activeLoans[j].toObject();
                QString dueDateString = loan["dueDate"].toString();
                QDate dueDate = QDate::fromString(dueDateString, "dd/MM/yyyy");

                if (currentDate > dueDate && loan["status"].toString() != "overdue") {
                    loan["status"] = "overdue";
                    activeLoans.replace(j, loan);
                    updateMade = true;
                }
            }
            user["activeLoans"] = activeLoans;
            userArray.replace(i, user);
        }
    }

    if (updateMade) {
        QJsonObject& database = getFileData();
        database["users"] = userArray;

        if (saveData()) {
            qDebug() << "ManagementTransaction: Updated overdue loans";
        } else {
            qDebug() << "ManagementTransaction: Failed to update overdue loans";
        }
    }
}

void ManagementTransaction::updateDueLoans(){
    QDate currentDate = QDate::currentDate();
    bool updateMade = false;

    for (int i = 0; i < userArray.size(); i++) {
        QJsonObject user = userArray[i].toObject();

        if (user.contains("activeLoans")) {
            QJsonArray activeLoans = user["activeLoans"].toArray();

            for (int j = 0; j < activeLoans.size(); j++) {
                QJsonObject loan = activeLoans[j].toObject();
                QString dueDateString = loan["dueDate"].toString();
                QDate dueDate = QDate::fromString(dueDateString, "dd/MM/yyyy");

                //check if the due date is within the next 7 days
                int daysToDue = currentDate.daysTo(dueDate);
                if (daysToDue > 0 && daysToDue <= 7 && loan["status"].toString() != "due") {
                    loan["status"] = "due";
                    activeLoans.replace(j, loan);
                    updateMade = true;
                }
            }
            user["activeLoans"] = activeLoans;
            userArray.replace(i, user);
        }
    }

    if (updateMade) {
        QJsonObject& database = getFileData();
        database["users"] = userArray;

        if (saveData()) {
            qDebug() << "ManagementTransaction: Updated due loans";
        } else {
            qDebug() << "ManagementTransaction: Failed to update due loans";
        }
    }
}
