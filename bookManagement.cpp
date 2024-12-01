#include "bookManagement.h"
#include "dataManagement.h"

// --------------- private ---------------


// --------------- protected ---------------

//constructor
BookManagement::BookManagement() : DataManagement() {}

// --------------- public ---------------

//getters
QJsonArray BookManagement::getBookArray() {
    readData();
    return libraryDatabase["books"].toArray();
}

QJsonObject BookManagement::getBookDetails(const QString &isbn){
    QJsonArray bookArray = getBookArray();
    for(int i = 0; i<bookArray.size(); i++){
        QJsonObject book = bookArray[i].toObject();
        if(book["isbn"].toString() == isbn){
            return book;
            break;
        }
    }
    qDebug()<<"BookManagement: Book Information not found.";
    return QJsonObject();
}

BookListView *BookManagement::createBookList(const QJsonObject &book, const QJsonObject &entry){
    // Create the BookListView widget
    BookListView* bookListView = new BookListView(nullptr);

    // Setting book details
    QLabel* titleLabel = bookListView->findChild<QLabel*>("titleOutputLabel");
    titleLabel->setText(book["title"].toString());

    QLabel* authorLabel = bookListView->findChild<QLabel*>("authorOutputLabel");
    authorLabel->setText(book["author"].toString());

    QLabel* isbnLabel = bookListView->findChild<QLabel*>("isbnOutputLabel");
    isbnLabel->setText(book["isbn"].toString());

    QLabel* dueLabel = bookListView->findChild<QLabel*>("dueOutputLabel");
    dueLabel->setText(entry["dueDate"].toString());

    // Set the cover image for the book
    QLabel* coverLabel = bookListView->findChild<QLabel*>("coverLabel");
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
        qDebug() << "BookManagement: Cover label not found";
    }
    return bookListView;
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

bool BookManagement::updateBook(const QString &isbn, const QJsonObject &updatedBook){

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
        qDebug()<<"BookManagement: Begining book Update";
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

        qDebug()<<"BookManagement: Failed to find book to update";
        return false;
    } else {
        qDebug()<<"BookManagement: No updates detected";
//        emit noUpdates();
        return false;
    }

}

//confirm book is available by checking isAvailable flag
//parameters : isbn of book to check.
//returns : true available/false not available
bool BookManagement::isAvailable(const QString& isbn) {
    QJsonArray bookArray = getBookArray();
    for(int i = 0; i<bookArray.size(); i++){
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

