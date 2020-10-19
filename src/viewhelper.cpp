#include "viewhelper.h"
#include "screenshot.h"

#include <QGuiApplication>
#include <QtQml>

#include <wayland-client.h>
#include <qpa/qplatformnativeinterface.h>
#include <sailfishapp.h>
#include <QDebug>

static void setWaylandInputRegion(QPlatformNativeInterface *wliface, QWindow *window, const QRegion &region)
{
    if (wl_compositor *wlcompositor = static_cast<wl_compositor *>(
                wliface->nativeResourceForIntegration("compositor"))) {
        if (wl_surface *wlsurface = static_cast<wl_surface *>(
                    wliface->nativeResourceForWindow("surface", window))) {
            wl_region *wlregion = wl_compositor_create_region(wlcompositor);

            for (const QRect &rect : region.rects()) {
                wl_region_add(wlregion, rect.x(), rect.y(), rect.width(), rect.height());
            }

            wl_surface_set_input_region(wlsurface, wlregion);
            wl_region_destroy(wlregion);

            wl_surface_commit(wlsurface);
        }
    }
}

static void setWaylandOpaqueRegion(QPlatformNativeInterface *wliface, QWindow *window, const QRegion &region)
{
    if (wl_compositor *wlcompositor = static_cast<wl_compositor *>(
                wliface->nativeResourceForIntegration("compositor"))) {
        if (wl_surface *wlsurface = static_cast<wl_surface *>(
                    wliface->nativeResourceForWindow("surface", window))) {
            wl_region *wlregion = wl_compositor_create_region(wlcompositor);

            for (const QRect &rect : region.rects()) {
                wl_region_add(wlregion, rect.x(), rect.y(), rect.width(), rect.height());
            }

            wl_surface_set_opaque_region(wlsurface, wlregion);
            wl_region_destroy(wlregion);

            wl_surface_commit(wlsurface);
        }
    }
}


ViewHelper::ViewHelper(QObject *parent) :
    QObject(parent)
{
    lastXPosConf = new MGConfItem("/apps/harbour-screentapshot/lastXPos", this);
    lastYPosConf = new MGConfItem("/apps/harbour-screentapshot/lastYPos", this);
    screenshotAnimationConf = new MGConfItem("/apps/harbour-screentapshot/screenshotAnimation", this);
    QObject::connect(screenshotAnimationConf, SIGNAL(valueChanged()), this, SIGNAL(screenshotAnimationChanged()));
    screenshotDelayConf = new MGConfItem("/apps/harbour-screentapshot/screenshotDelay", this);
    QObject::connect(screenshotDelayConf, SIGNAL(valueChanged()), this, SIGNAL(screenshotDelayChanged()));
    useSubfolderConf = new MGConfItem("/apps/harbour-screentapshot/useSubfolder", this);
    QObject::connect(useSubfolderConf, SIGNAL(valueChanged()), this, SIGNAL(useSubfolderChanged()));
    orientationLockConf = new MGConfItem("/lipstick/orientationLock", this);
    QObject::connect(orientationLockConf, SIGNAL(valueChanged()), this, SIGNAL(orientationLockChanged()));

    overlayView = NULL;
    settingsView = NULL;
}

bool ViewHelper::eventFilter(QObject *object, QEvent *event)
{
    if (object != overlayView) {
        return false;
    } else switch (event->type()) {
    case QEvent::PlatformSurface: {
        QPlatformSurfaceEvent *platformEvent = static_cast<QPlatformSurfaceEvent *>(event);

        if (QPlatformWindow *handle = platformEvent->surfaceEventType() == QPlatformSurfaceEvent::SurfaceCreated
                ? overlayView->handle()
                : nullptr) {
            QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();

            native->setWindowProperty(handle, QStringLiteral("CATEGORY"), QStringLiteral("notification"));
            setWaylandOpaqueRegion(native, overlayView, QRegion(0, 0, 0, 0));
        }
        return false;
    }
    case QEvent::DynamicPropertyChange: {
        QDynamicPropertyChangeEvent *propertyEvent = static_cast<QDynamicPropertyChangeEvent *>(event);
        if (QPlatformWindow *handle = propertyEvent->propertyName() == "MOUSE_REGION"
                ? overlayView->handle()
                : nullptr) {
            QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();

            auto mouseRegion = overlayView->property("MOUSE_REGION");
            native->setWindowProperty(handle, QStringLiteral("MOUSE_REGION"), mouseRegion);
            setWaylandInputRegion(native, overlayView, mouseRegion.value<QRegion>());
        }
        return false;
    }
    default:
        return false;
    }

}

void ViewHelper::closeOverlay()
{
    if (overlayView) {
        QDBusConnection::sessionBus().unregisterObject("/harbour/screentapshot/overlay");
        QDBusConnection::sessionBus().unregisterService("harbour.screentapshot.overlay");
        overlayView->removeEventFilter(this);
        overlayView->close();
        delete overlayView;
        overlayView = NULL;
    }
    else {
        QDBusInterface iface("harbour.screentapshot.overlay", "/harbour/screentapshot/overlay", "harbour.screentapshot");
        iface.call(QDBus::NoBlock, "exit");
    }
}

void ViewHelper::openStore()
{
    QDBusInterface iface("com.jolla.jollastore", "/StoreClient", "com.jolla.jollastore");
    iface.call(QDBus::NoBlock, "showApp", "harbour-screentapshot");
}

void ViewHelper::setDefaultRegion()
{
    setMouseRegion(QRegion(lastXPos(),
                           lastYPos(),
                           80, 80));
}

void ViewHelper::checkActiveSettings()
{
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

void ViewHelper::checkActiveOverlay()
{
    bool inactive = QDBusConnection::sessionBus().registerService("harbour.screentapshot.overlay");
    if (inactive) {
        showOverlay();
        QDBusConnection::systemBus().connect(
            QString(),
            QString(),
            QStringLiteral("org.freedesktop.PackageKit.Transaction"),
            QStringLiteral("Package"),
            this,
            SLOT(onPackageKitPackage(quint32, QString, QString)));
    }
}

void ViewHelper::setTouchRegion(const QRect &rect, bool setXY)
{
    setMouseRegion(QRegion(rect));

    if (setXY) {
        setLastXPos(rect.x());
        setLastYPos(rect.y());
    }
}

void ViewHelper::show()
{
    if (settingsView) {
        settingsView->raise();
        checkActiveOverlay();
    }
}

void ViewHelper::exit()
{
    qGuiApp->quit();
}

void ViewHelper::showOverlay()
{
    QDBusConnection::sessionBus().registerObject("/harbour/screentapshot/overlay", this, QDBusConnection::ExportScriptableSlots);

    qmlRegisterType<Screenshot>("harbour.screentapshot2.screenshot", 1, 0, "Screenshot");

    qGuiApp->setApplicationName("ScreenTapShot");
    qGuiApp->setApplicationDisplayName("ScreenTapShot");

    overlayView = SailfishApp::createView();
    overlayView->installEventFilter(this);

    QObject::connect(overlayView->engine(), SIGNAL(quit()), qGuiApp, SLOT(quit()));
    overlayView->setTitle("ScreenTapShot");
    overlayView->rootContext()->setContextProperty("viewHelper", this);

    QColor color;
    color.setRedF(0.0);
    color.setGreenF(0.0);
    color.setBlueF(0.0);
    color.setAlphaF(0.0);
    overlayView->setColor(color);
    overlayView->setClearBeforeRendering(true);

    overlayView->setSource(SailfishApp::pathTo("qml/overlay.qml"));
    overlayView->create();
    setDefaultRegion();
    overlayView->show();
}

void ViewHelper::showSettings()
{
    QDBusConnection::sessionBus().registerObject("/harbour/screentapshot/settings", this, QDBusConnection::ExportScriptableSlots);

    qGuiApp->setApplicationName("ScreenTapShot Settings");
    qGuiApp->setApplicationDisplayName("ScreenTapShot Settings");

    settingsView = SailfishApp::createView();
    settingsView->setTitle("ScreenTapShot Settings");
    settingsView->rootContext()->setContextProperty("viewHelper", this);
    settingsView->setSource(SailfishApp::pathTo("qml/settings.qml"));
    settingsView->showFullScreen();

    QObject::connect(settingsView, SIGNAL(destroyed()), this, SLOT(onSettingsDestroyed()));
    QObject::connect(settingsView, SIGNAL(closing(QQuickCloseEvent*)), this, SLOT(onSettingsClosing(QQuickCloseEvent*)));
}

void ViewHelper::setMouseRegion(const QRegion &region)
{
    overlayView->setMask(region);
    overlayView->setProperty("MOUSE_REGION", region);
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

bool ViewHelper::useSubfolder()
{
    return useSubfolderConf->value(false).toBool();
}

void ViewHelper::setUseSubfolder(bool value)
{
    useSubfolderConf->set(value);
    Q_EMIT useSubfolderChanged();
}

QString ViewHelper::orientationLock() const
{
    return orientationLockConf->value(QString("dynamic")).toString();
}

void ViewHelper::onPackageKitPackage(quint32 status, const QString &pkgId, const QString &)
{
    if (status == 13 && pkgId.startsWith(QLatin1String("harbour-screentapshot2;"))) {
        Q_EMIT applicationRemoval();
    }
}

void ViewHelper::onSettingsDestroyed()
{
    QObject::disconnect(settingsView, 0, 0, 0);
    settingsView = NULL;
}

void ViewHelper::onSettingsClosing(QQuickCloseEvent *)
{
    settingsView->destroy();
    settingsView->deleteLater();

    QDBusConnection::sessionBus().unregisterObject("/harbour/screentapshot/settings");
    QDBusConnection::sessionBus().unregisterService("harbour.screentapshot.settings");
}
