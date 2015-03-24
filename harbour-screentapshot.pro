TARGET = harbour-screentapshot

QT += gui-private dbus
CONFIG += sailfishapp link_pkgconfig
PKGCONFIG += sailfishapp mlite5

DEFINES += APP_VERSION=\\\"$$VERSION\\\"

images.files = images
images.path = /usr/share/harbour-screentapshot

INSTALLS += images

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
    qml/overlay.qml \
    qml/settings.qml \
    qml/CoverPage.qml \
    qml/MainPage.qml \
    qml/AboutPage.qml
