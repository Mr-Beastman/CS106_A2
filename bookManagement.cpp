#include "bookManagement.h"
#include "dataManagement.h"

// --------------- private ---------------

//intializing static memebers
BookManagement * BookManagement::bookManager = nullptr;
QMutex BookManagement::bookMtx;

// --------------- protected ---------------

//constructor
BookManagement::BookManagement() : DataManagement() {}

// --------------- public ---------------

//setters
void BookManagement::setBookArray(){
    if(libraryDatabase.isEmpty()){
        readData();
    }
    bookArray = libraryDatabase["books"].toArray();
}

//getters
QJsonArray& BookManagement::getBookArray() {
    return bookArray;
}

QJsonObject BookManagement::getBookDetails(const QString &isbn){
    for(int i = 0; bookArray.size(); i++){
        QJsonObject book = bookArray[i].toObject();
        qDebug()<<book["title"].toString();
        if(book["isbn"].toString() == isbn){
            return book;
            break;
        }
    }
    qDebug()<<"BookManagement: Book Information not found.";
    return QJsonObject();
}

//getter for bookManager singleton
BookManagement *BookManagement::getBookManager() {
    if(bookManager == nullptr) {
        QMutexLocker locker(&bookMtx);
        if(bookManager == nullptr){
            bookManager = new BookManagement();
        }
    }
    return bookManager;
}

//methods

//add book to Json file database
//parameters : book title, author, isbn, description, genre and section.
//returns : true if ran successfully, false if errror encountered.
bool BookManagement::addBook(const QString& titleInput,
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
    bookArray.append(newBook);
    libraryDatabase["books"] = bookArray;

    //save updates to json file
    if(saveData()){
        qDebug()<<"New book has been added";
        return true;
    }

    qDebug()<<"BookManagment: Failed to save new book";
    return false;
}

//confirm book is available by checking isAvailable flag
//parameters : isbn of book to check.
//returns : true available/false not available
bool BookManagement::isAvailable(const QString& isbn) {

    for(int i = 0; bookArray.size(); i++){
        QJsonObject book = bookArray[i].toObject();
        if(book["isbn"].toString() == isbn){
            return book["isAvailable"].toBool();
        }
    }

    qDebug()<<"BookManagement: "<<isbn<<" not found";
    return false;
}

//identify file path
//pareamters : none
//returns : string containing folder path
QString BookManagement::findCoverPath(){
    return findPath()+QDir::separator()+"/images/covers/";
}

//get path to requested cover image
//parameters: the isbn number of the book requestes
//returns: none
QString BookManagement::findCoverImage(QString& isbn){
    return findCoverPath()+isbn+".png";
}

