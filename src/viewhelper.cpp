#include "viewhelper.h"

#include <QGuiApplication>
#include <qpa/qplatformnativeinterface.h>
#include <QDebug>

ViewHelper::ViewHelper(QQuickView *parent) :
    QObject(parent),
    view(parent)
{
    lastXPosConf = new MGConfItem("/apps/harbour-screentapshot/lastXPos", this);
    lastYPosConf = new MGConfItem("/apps/harbour-screentapshot/lastYPos", this);
}

void ViewHelper::setDefaultRegion()
{
    setMouseRegion(QRegion(lastXPosConf->value(0).toInt(),
                           lastYPosConf->value(0).toInt(),
                           80, 80));
}

void ViewHelper::setTouchRegion(const QRect &rect)
{
    setMouseRegion(QRegion(rect));

    setLastXPos(rect.x());
    setLastYPos(rect.y());
}

void ViewHelper::detachWindow()
{
    view->close();
    view->create();

    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();
    native->setWindowProperty(view->handle(), QLatin1String("CATEGORY"), "notification");
    setDefaultRegion();

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
