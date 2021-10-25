# Version=0.0.1
#
#
#Dependencies -
# MatildaIO


INCLUDEPATH  += $$PWD\
                $$PWD/../../defines/defines
                $$PWD/../../defines/define-types

HEADERS += \
    $$PWD/localservers/regularlocalserver.h \
    $$PWD/localsockets/regularlocalsocket.h \
    $$PWD/localsockets/regularserversocket.h

SOURCES += \
    $$PWD/localservers/regularlocalserver.cpp \
    $$PWD/localsockets/regularlocalsocket.cpp \
    $$PWD/localsockets/regularserversocket.cpp
