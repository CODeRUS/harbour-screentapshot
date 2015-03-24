#include "viewhelper.h"
#include "screenshot.h"

#include <QGuiApplication>
#include <QtQml>
#include <qpa/qplatformnativeinterface.h>
#include <sailfishapp.h>
#include <QDebug>

ViewHelper::ViewHelper(QObject *parent) :
    QObject(parent)
{
    lastXPosConf = new MGConfItem("/apps/harbour-screentapshot/lastXPos", this);
    lastYPosConf = new MGConfItem("/apps/harbour-screentapshot/lastYPos", this);
    screenshotAnimationConf = new MGConfItem("/apps/harbour-screentapshot/screenshotAnimation", this);
    QObject::connect(screenshotAnimationConf, SIGNAL(valueChanged()), this, SIGNAL(screenshotAnimationChanged()));
    screenshotDelayConf = new MGConfItem("/apps/harbour-screentapshot/screenshotDelay", this);
    QObject::connect(screenshotDelayConf, SIGNAL(valueChanged()), this, SIGNAL(screenshotDelayChanged()));
}

void ViewHelper::closeOverlay()
{
    QDBusInterface iface("harbour.screentapshot.overlay", "/harbour/screentapshot/overlay", "harbour.screentapshot");
    iface.call(QDBus::NoBlock, "exit");
}

void ViewHelper::setDefaultRegion()
{
    setMouseRegion(QRegion(lastXPosConf->value(0).toInt(),
                           lastYPosConf->value(0).toInt(),
                           80, 80));
}

void ViewHelper::checkActive()
{
    bool inactive = QDBusConnection::sessionBus().registerService("harbour.screentapshot.overlay");
    if (inactive) {
        showOverlay();
    }
    else {
        bool newSettings = QDBusConnection::sessionBus().registerService("harbour.screentapshot.settings");
        if (newSettings) {
            showSettings();
        }
        else {
            QDBusInterface iface("harbour.screentapshot.settings", "/harbour/screentapshot/settings", "harbour.screentapshot");
            iface.call(QDBus::NoBlock, "show");
            qGuiApp->exit(0);
            return;
        }
    }
}

void ViewHelper::setTouchRegion(const QRect &rect)
{
    setMouseRegion(QRegion(rect));

    setLastXPos(rect.x());
    setLastYPos(rect.y());
}

void ViewHelper::show()
{
    if (view) {
        view->raise();
    }
}

void ViewHelper::exit()
{
    qGuiApp->quit();
}

void ViewHelper::showOverlay()
{
    QDBusConnection::sessionBus().registerObject("/harbour/screentapshot/overlay", this, QDBusConnection::ExportScriptableSlots);

    qmlRegisterType<Screenshot>("harbour.screentapshot.screenshot", 1, 0, "Screenshot");

    qGuiApp->setApplicationName("ScreenTapShot");
    qGuiApp->setApplicationDisplayName("ScreenTapShot");

    view = SailfishApp::createView();
    QObject::connect(view->engine(), SIGNAL(quit()), qGuiApp, SLOT(quit()));
    view->setTitle("ScreenTapShot");
    view->rootContext()->setContextProperty("viewHelper", this);

    QColor color;
    color.setRedF(0.0);
    color.setGreenF(0.0);
    color.setBlueF(0.0);
    color.setAlphaF(0.0);
    view->setColor(color);
    view->setClearBeforeRendering(true);

    view->setSource(SailfishApp::pathTo("qml/overlay.qml"));
    view->show();
    view->close();
    view->create();
    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();
    native->setWindowProperty(view->handle(), QLatin1String("CATEGORY"), "notification");
    setDefaultRegion();

    view->showFullScreen();
}

void ViewHelper::showSettings()
{
    QDBusConnection::sessionBus().registerObject("/harbour/screentapshot/settings", this, QDBusConnection::ExportScriptableSlots);

    qGuiApp->setApplicationName("ScreenTapShot Settings");
    qGuiApp->setApplicationDisplayName("ScreenTapShot Settings");

    view = SailfishApp::createView();
    view->setTitle("ScreenTapShot Settings");
    view->rootContext()->setContextProperty("viewHelper", this);
    view->setSource(SailfishApp::pathTo("qml/settings.qml"));
    view->showFullScreen();
}

void ViewHelper::setMouseRegion(const QRegion &region)
{
    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();
    native->setWindowProperty(view->handle(), QLatin1String("MOUSE_REGION"), region);
}

int ViewHelper::lastXPos()
{
    return lastXPosConf->value(0).toInt();
}

void ViewHelper::setLastXPos(int xpos)
{
    lastXPosConf->set(xpos);
    Q_EMIT lastXPosChanged();
}

int ViewHelper::lastYPos()
{
    return lastYPosConf->value(0).toInt();
}

void ViewHelper::setLastYPos(int ypos)
{
    lastYPosConf->set(ypos);
    Q_EMIT lastYPosChanged();
}

bool ViewHelper::screenshotAnimation()
{
    return screenshotAnimationConf->value(true).toBool();
}

void ViewHelper::setScreenshotAnimation(bool value)
{
    screenshotAnimationConf->set(value);
    Q_EMIT screenshotAnimationChanged();
}

int ViewHelper::screenshotDelay()
{
    return screenshotDelayConf->value(3).toInt();
}

void ViewHelper::setScreenshotDelay(int value)
{
    screenshotDelayConf->set(value);
    Q_EMIT screenshotDelayChanged();
}
