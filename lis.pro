QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    addBookview.cpp \
    adminView.cpp \
    bookInfoView.cpp \
    bookListView.cpp \
    bookManagement.cpp \
    dataManagement.cpp \
    loginView.cpp \
    main.cpp \
    mainWindow.cpp \
    memberView.cpp \
    modifyBookview.cpp \
    registrationView.cpp \
    transactionManagement.cpp \
    updateUserView.cpp \
    userManagement.cpp

HEADERS += \
    addBookview.h \
    adminView.h \
    bookInfoView.h \
    bookListView.h \
    bookManagement.h \
    dataManagement.h \
    loginView.h \
    mainWindow.h \
    memberView.h \
    modifyBookview.h \
    registrationView.h \
    transactionManagement.h \
    updateUserView.h \
    userManagement.h

FORMS += \
    addBookView.ui \
    adminView.ui \
    bookInfoView.ui \
    bookListView.ui \
    loginView.ui \
    mainWindow.ui \
    memberView.ui \
    modifyBookView.ui \
    registrationView.ui \
    updateUserView.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    loginDatabase.db

RESOURCES += \
    images.qrc
