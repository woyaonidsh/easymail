QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    addfriend.cpp \
    confirm_operation.cpp \
    lookemail.cpp \
    main.cpp \
    login.cpp \
    mainwindow.cpp \
    previewpage.cpp \
    registe.cpp \
    sentemail.cpp \

HEADERS += \
    addfriend.h \
    confirm_operation.h \
    login.h \
    lookemail.h \
    mainwindow.h \
    previewpage.h \
    registe.h \
    sentemail.h \

FORMS += \
    addfriend.ui \
    confirm_operation.ui \
    login.ui \
    lookemail.ui \
    mainwindow.ui \
    previewpage.ui \
    registe.ui \
    sentemail.ui \

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resource.qrc
