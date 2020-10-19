THEMENAME = sailfish-default

TARGET = harbour-screentapshot2

QT += dbus gui-private
CONFIG += sailfishapp
PKGCONFIG += \
    mlite5 \
    wayland-client \
# PKGCONFIG end

DEFINES += APP_VERSION=\\\"$$VERSION\\\"

images.files = images
images.path = /usr/share/$$TARGET

INSTALLS += images

SOURCES += \
    src/screenshot.cpp \
    src/viewhelper.cpp \
    src/main.cpp

HEADERS += \
    src/viewhelper.h \
    src/screenshot.h

OTHER_FILES += \
    rpm/harbour-screentapshot2.spec \
    harbour-screentapshot2.desktop \
    qml/overlay.qml \
    qml/settings.qml \
    qml/CoverPage.qml \
    qml/MainPage.qml \
    qml/AboutPage.qml \
    translations/*.ts

privileges.files = privileges/harbour-screentapshot2
privileges.path = /usr/share/mapplauncherd/privileges.d
INSTALLS += privileges

# to disable building translations every time, comment out the
# following CONFIG line
CONFIG += sailfishapp_i18n

TRANSLATIONS += \
    translations/harbour-screentapshot2-es.ts \
    translations/harbour-screentapshot2-fi.ts \
    translations/harbour-screentapshot2-pl.ts \
    translations/harbour-screentapshot2-pt_BR.ts \
    translations/harbour-screentapshot2-pt.ts \
    translations/harbour-screentapshot2-ru.ts \
    translations/harbour-screentapshot2-sv.ts \
    translations/harbour-screentapshot2-zh_CN.ts

appicon.profiles = \
    86x86 \
    108x108 \
    128x128 \
    256x256

appicon.scales.86x86 = 1.0
appicon.scales.108x108 = 1.25
appicon.scales.128x128 = 1.49
appicon.scales.256x256 = 2.97

for(profile, appicon.profiles) {
    system(mkdir -p $${OUT_PWD}/$${profile})

    appicon.commands += /usr/bin/sailfish_svg2png \
        -z $$eval(appicon.scales.$${profile}) \
         $${_PRO_FILE_PWD_}/appicon \
         $${profile}/apps/ &&

    appicon.files += $${profile}
}
appicon.commands += true
appicon.path = /usr/share/icons/hicolor/

INSTALLS += appicon

CONFIG += sailfish-svg2png
