#include "screenshot.h"

#include <QDateTime>

Screenshot::Screenshot(QObject *parent) :
    QObject(parent)
{
    iface = new QDBusInterface("org.nemomobile.lipstick",
                               "/org/nemomobile/lipstick/screenshot",
                               "org.nemomobile.lipstick",
                               QDBusConnection::sessionBus(), this);
}

void Screenshot::capture()
{
    pendingScreenshot = QString("/home/nemo/Pictures/Screenshot-%1.png").arg(QDateTime::currentDateTime().toString("dd-MM-yy-hh-mm-ss"));

    QDBusPendingCall async = iface->asyncCall("saveScreenshot", pendingScreenshot);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(async, this);

    QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                     this, SLOT(onCaptureFinished(QDBusPendingCallWatcher*)));
}

void Screenshot::onCaptureFinished(QDBusPendingCallWatcher *call)
{
    if (call->error().type() == QDBusError::NoError) {
        Q_EMIT captured(pendingScreenshot);
    }
}
