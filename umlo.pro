VERSION = 1.0.0
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
QT       += core gui concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
# Specifies name of the binary.
TARGET = umlo

# Denotes that project is an application.
TEMPLATE = app

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
unix {
    isEmpty(PREFIXINS) {
        PREFIXINS = /usr/share
    }
    shortcutfiles.path = $$(PREFIXINS)/applications/
    shortcutfiles.files = $$PWD/umlo.desktop
    iconfiles.path = $$(PREFIXINS)/icons/
    iconfiles.files = $$PWD/images/umlo.png
    docfiles.path = $$(PREFIXINS)/doc/umlo
    docfiles.files += $$PWD/README.md
    licfiles.path = $$(PREFIXINS)/licenses/umlo
    licfiles.files += $$PWD/LICENSE
    translationfiles.path = $$(PREFIXINS)/umlo/translations
    translationfiles.files += $$PWD/*.qm
    }

SOURCES += \
    about.cpp \
    main.cpp \
    parametres.cpp \
    umlo.cpp

HEADERS += \
    about.h \
    parametres.h \
    umlo.h

FORMS += \
    about.ui \
    parametres.ui \
    umlo.ui

TRANSLATIONS += \
    umlo_fr_FR.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = $$(PREFIXINS)/../bin
!isEmpty(target.path): INSTALLS += target shortcutfiles iconfiles translationfiles docfiles licfiles

DISTFILES += \
    README.md \
    umlo.desktop \
    images/umlo.ico \
    images/umlo.png
