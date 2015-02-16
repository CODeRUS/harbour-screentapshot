TARGET = harbour-screentapshot

QT += gui-private dbus
CONFIG += sailfishapp link_pkgconfig
PKGCONFIG += sailfishapp mlite5

SOURCES += \
    src/screenshot.cpp \
    src/viewhelper.cpp \
    src/main.cpp

HEADERS += \
    src/viewhelper.h \
    src/screenshot.h

OTHER_FILES += \
    rpm/harbour-screentapshot.spec \
    harbour-screentapshot.desktop \
    qml/main.qml \
    qml/SourceProxy.qml \
    qml/GaussianBlur.qml \
    qml/GaussianDirectionalBlur.qml
