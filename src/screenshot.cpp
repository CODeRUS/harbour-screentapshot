#include "screenshot.h"

#include <QDateTime>
#include <QStandardPaths>

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
    pendingScreenshot = QString("%1/Screenshot-%2.png")
            .arg(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation))
            .arg(QDateTime::currentDateTime().toString("dd-MM-yy-hh-mm-ss"));

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
