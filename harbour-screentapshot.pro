TARGET = harbour-screentapshot

QT += gui-private dbus
CONFIG += sailfishapp link_pkgconfig
PKGCONFIG += sailfishapp mlite5

SOURCES += \
    src/screenshot.cpp \
    src/viewhelper.cpp \
    src/main.cpp

OTHER_FILES += \
    rpm/harbour-screentapshot.spec \
    harbour-screentapshot.desktop \
    qml/main.qml

HEADERS += \
    src/viewhelper.h \
    src/screenshot.h
