#ifndef SCREENSHOT_H
#define SCREENSHOT_H

#include <QObject>

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusPendingCallWatcher>

class Screenshot : public QObject
{
    Q_OBJECT
public:
    explicit Screenshot(QObject *parent = 0);

public slots:
    void capture();

private slots:
    void onCaptureFinished(QDBusPendingCallWatcher *call);

signals:
    void captured(const QString &path);

private:
    QDBusInterface *iface;
    QString pendingScreenshot;
};

#endif // SCREENSHOT_H
