VERSION = 1.0.10
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
OUTPUT += Console

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    parametres.cpp \
    umlo.cpp

HEADERS += \
    parametres.h \
    umlo.h

FORMS += \
    parametres.ui \
    umlo.ui

TRANSLATIONS += \
    umlo_fr_FR.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
