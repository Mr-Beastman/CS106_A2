#include "bookManagement.h"
#include "dataManagement.h"

// --------------- private ---------------

//constructor
BookManagement::BookManagement() : DataManagement(){}

//intializing static memebers
BookManagement * BookManagement::bookManager = nullptr;
QMutex BookManagement::bookMtx;

// --------------- public ---------------

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

//add book to Json file database
//parameters : book title, author, isbn, description, genre and section.
//returns : none
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


    //add the new book to the existing array
    QJsonArray bookArray = jsonData["books"].toArray();
    bookArray.append(newBook);
    jsonData["books"] =bookArray;

    //save updates to json file
    if(saveData()){
        qDebug()<<"New book has been added";
        return true;
    }

    qDebug()<<"BookManagment: Failed to save new book";
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

