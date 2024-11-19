#include "bookManagement.h"
#include "dataManagement.h"

bookManagement::bookManagement(const QString& dbPath) : DataManagement(){
    readData();
}

bool bookManagement::addBook(const QString& titleInput,
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


    //add newUser to the existing array
    QJsonArray bookArray = jsonData["books"].toArray();
    bookArray.append(newBook);
    jsonData["books"] =bookArray;

    //save updates to json file
    if(saveData()){
        qDebug()<<"New book has been added";
        return true;
    }

    qDebug()<<"Failed to save new book";
    return false;
}
