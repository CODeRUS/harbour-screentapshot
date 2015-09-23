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

    Q_PROPERTY(bool useSubfolder READ useSubfolder WRITE setUseSubfolder NOTIFY useSubfolderChanged)

public slots:
    void capture();

private slots:
    void onCaptureFinished(QDBusPendingCallWatcher *call);

signals:
    void useSubfolderChanged();

    void captured(const QString &path);

private:
    bool useSubfolder();
    void setUseSubfolder(bool value);
    bool _useSubfolder;

    QDBusInterface *iface;
    QString pendingScreenshot;
};

#endif // SCREENSHOT_H
