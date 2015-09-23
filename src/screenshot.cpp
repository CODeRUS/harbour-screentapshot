#include "screenshot.h"

#include <QDateTime>
#include <QStandardPaths>
#include <QDir>

Screenshot::Screenshot(QObject *parent) :
    QObject(parent),
    _useSubfolder(false)
{
    iface = new QDBusInterface("org.nemomobile.lipstick",
                               "/org/nemomobile/lipstick/screenshot",
                               "org.nemomobile.lipstick",
                               QDBusConnection::sessionBus(), this);
}

void Screenshot::capture()
{
    QString folder = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    if (_useSubfolder) {
        folder.append("/Screenshots");
    }

    QDir test;
    if (!test.exists(folder)) {
        test.mkpath(folder);
    }

    pendingScreenshot = QString("%1/Screenshot-%2.png")
            .arg(folder)
            .arg(QDateTime::currentDateTime().toString("yy-MM-dd-hh-mm-ss"));

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

bool Screenshot::useSubfolder()
{
    return _useSubfolder;
}

void Screenshot::setUseSubfolder(bool value)
{
    if (_useSubfolder != value) {
        _useSubfolder = value;
        Q_EMIT useSubfolderChanged();
    }
}
