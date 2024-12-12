#include "managementBook.h"
#include "managementUser.h"


//constructor
ManagementBook::ManagementBook() : ManagementData() {
    setBookArray();
}

void ManagementBook::setBookArray(){
    QJsonObject& database = getFileData();
    bookArray = database["books"].toArray();
}


//getters
QJsonArray ManagementBook::getBookArray() {
    return bookArray;
}

QJsonObject ManagementBook::getBookDetails(const QString &isbn){
    QJsonArray bookArray = getBookArray();
    for(int i = 0; i<bookArray.size(); i++){
        QJsonObject book = bookArray[i].toObject();
        if(book["isbn"].toString() == isbn){
            return book;
            break;
        }
    }
    qDebug()<<"ManagementBook: Book Information not found.";
    return QJsonObject();
}

ViewBookItem *ManagementBook::createBookList(const QJsonObject &book, const QJsonObject &entry){
    // Create the viewBookItem widget
    ViewBookItem* viewBookItem = new ViewBookItem(nullptr);

    // Setting book details
    QLabel* titleLabel = viewBookItem->findChild<QLabel*>("titleOutputLabel");
    titleLabel->setText(book["title"].toString());

    QLabel* authorLabel = viewBookItem->findChild<QLabel*>("authorOutputLabel");
    authorLabel->setText(book["author"].toString());

    QLabel* isbnLabel = viewBookItem->findChild<QLabel*>("isbnOutputLabel");
    isbnLabel->setText(book["isbn"].toString());

    QLabel* dueLabel = viewBookItem->findChild<QLabel*>("dueOutputLabel");
    dueLabel->setText(entry["dueDate"].toString());

    // Set the cover image for the book
    QLabel* coverLabel = viewBookItem->findChild<QLabel*>("coverLabel");
    if (coverLabel) {
        QString coverImagePath = findCoverPath() + book["isbn"].toString() + ".png";
        QPixmap cover(coverImagePath);
        QString noCoverImagePath = findCoverPath() + "noCover.png";
        QPixmap noCover(noCoverImagePath);

        if (!cover.isNull()) {
            coverLabel->setPixmap(cover);
        } else {
            coverLabel->setPixmap(noCover);
        }
        coverLabel->setScaledContents(true);
    } else {
        qDebug() << "ManagementBook: Cover label not found";
    }
    return viewBookItem;
}

//methods

//add book to Json file database
//parameters : book title, author, isbn, description, genre and section.
//returns : true if ran successfully, false if errror encountered.
bool ManagementBook::addBook(const QString& titleInput,
                             const QString& authorInput,
                             const QString& isbnInput,
                             const QString& descInput,
                             const QString& genreInput,
                             const QString& sectInput){

    //creating Json obj with new user details;
    QJsonObject newBook;
    newBook["title"] = titleInput;
    newBook["author"] = authorInput;
    newBook["isbn"] = isbnInput;
    newBook["desc"] = descInput;
    newBook["genre"] = genreInput;
    newBook["sect"] = sectInput;
    newBook["inQueue"] = QJsonArray();
    newBook["isAvailable"] = true;

    //add the new book to the existing array
    QJsonArray bookArray = getBookArray();
    bookArray.append(newBook);
    libraryDatabase["books"] = bookArray;

    //save updates to json file
    if(saveData()){
        qDebug()<<"BookManagemt: New book has been added";
        return true;
    }

    qDebug()<<"BookManagment: Failed to save new book";
    return false;
}

bool ManagementBook::updateBook(const QString &isbn, const QJsonObject &updatedBook){

    QJsonObject originalBook = getBookDetails(isbn);

    //checking for changes
    bool updated = false;

    if(originalBook["title"].toString() != updatedBook["title"].toString()){
        updated = true;
    }
    if(originalBook["author"].toString() != updatedBook["author"].toString()){
        updated = true;
    }
    if(originalBook["isbn"].toString() != updatedBook["isbn"].toString()){
        updated = true;
    }
    if(originalBook["genre"].toString() != updatedBook["genre"].toString()){
        updated = true;
    }
    if(originalBook["sect"].toString() != updatedBook["sect"].toString()){
        updated = true;
    }
    if(originalBook["desc"].toString() != updatedBook["desc"].toString()){
        updated = true;
    }

    //if update loop through curent data to update in array
    if(updated){
        qDebug()<<"ManagementBook: Begining book Update";
        QJsonArray bookArray = getBookArray();
        for(int i = 0; i < bookArray.size(); i++){
            QJsonObject book = bookArray[i].toObject();

            if(book["isbn"].toString() == isbn) {
                book["title"] = updatedBook["title"];
                book["author"] = updatedBook["author"];
                book["isbn"] = updatedBook["isbn"];
                book["genre"] = updatedBook["genre"];
                book["sect"] = updatedBook["sect"];
                book["desc"] = updatedBook["desc"];

                bookArray[i] = book;

                libraryDatabase["books"]=bookArray;

                //save updates to json file
                if(saveData()){
                    qDebug()<<"BookManagemt: Book has been updated";
                    return true;
                }

                qDebug()<<"BookManagment: Failed to update book";
                return false;
            }
        }

        qDebug()<<"ManagementBook: Failed to find book to update";
        return false;
    } else {
        qDebug()<<"ManagementBook: No updates detected";
//        emit noUpdates();
        return false;
    }

}

//confirm book is available by checking isAvailable flag
//parameters : isbn of book to check.
//returns : true available/false not available
bool ManagementBook::isAvailable(const QString& isbn) {
    QJsonArray bookArray = getBookArray();
    for(int i = 0; i<bookArray.size(); i++){
        QJsonObject book = bookArray[i].toObject();
        if(book["isbn"].toString() == isbn){
            qDebug()<<"ManagementBook: "<<isbn<<" is availble : "<<book["isAvailable"].toBool();
            return book["isAvailable"].toBool();
        }
    }

    qDebug()<<"ManagementBook: "<<isbn<<" not found";
    return false;
}

bool ManagementBook::isIssued(const QString &isbn){
    for(int i = 0; i<bookArray.size(); i++){
        QJsonObject book = bookArray[i].toObject();
        if(book["isbn"].toString() == isbn){
            if(book["issuedTo"].toString().isEmpty()){
                qDebug()<<"ManagementBook: "<<isbn<<" is not issued";
                return false;
            } else {
                qDebug()<<"ManagementBook: "<<isbn<<" is issued to "<<book["issuedTo"];
                return true;
            }
        }
    }

    qDebug()<<"ManagementBook: "<<isbn<<" not found";
    return false;
}

QString ManagementBook::getDueDate(const QString &username, const QString &isbn){
    ManagementUser userManager;

    QJsonObject user = userManager.getUserObj(username);

    QJsonArray loans = user["activeLoans"].toArray();

    for(int i = 0; i<loans.size(); i++){
        QJsonObject loan = loans[i].toObject();

        if(loan["isbn"].toString() == isbn){
            return loan["dueDate"].toString();
        }
    }

    qDebug()<<"ManagementBook: "<<isbn<<" due date could not be confirmed";
    return QString();

}

//identify file path
//pareamters : none
//returns : string containing folder path
QString ManagementBook::findCoverPath(){
    return findPath()+QDir::separator()+"/images/covers/";
}

//get path to requested cover image
//parameters: the isbn number of the book requestes
//returns: none
QString ManagementBook::findCoverImage(QString& isbn){
    return findCoverPath()+isbn+".png";
}

