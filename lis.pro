QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainWindow.cpp \
    managementBook.cpp \
    managementData.cpp \
    managementTransaction.cpp \
    managementUser.cpp \
    viewAddBook.cpp \
    viewAddMember.cpp \
    viewAdminDashboard.cpp \
    viewBookInfo.cpp \
    viewBookItem.cpp \
    viewLogin.cpp \
    viewMemberDashboard.cpp \
    viewMemberInfo.cpp \
    viewRegistration.cpp \
    viewUpdateBook.cpp \
    viewUpdateUser.cpp

HEADERS += \
    mainWindow.h \
    managementBook.h \
    managementData.h \
    managementTransaction.h \
    managementUser.h \
    viewAddBook.h \
    viewAddMember.h \
    viewAdminDashboard.h \
    viewBookInfo.h \
    viewBookItem.h \
    viewLogin.h \
    viewMemberDashboard.h \
    viewMemberInfo.h \
    viewRegistration.h \
    viewUpdateBook.h \
    viewUpdateUser.h

FORMS += \
    mainWindow.ui \
    viewAddBook.ui \
    viewAddMember.ui \
    viewAdminDashboard.ui \
    viewBookInfo.ui \
    viewBookItem.ui \
    viewLogin.ui \
    viewMemberDashboard.ui \
    viewMemberInfo.ui \
    viewRegistration.ui \
    viewUpdateBook.ui \
    viewUpdateUser.ui


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    loginDatabase.db

RESOURCES += \
    images.qrc

