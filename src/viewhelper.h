#ifndef VIEWHELPER_H
#define VIEWHELPER_H

#include <QObject>
#include <QQuickView>
#include <mlite5/MGConfItem>
#include <QDBusInterface>
#include <QDBusConnection>

class ViewHelper : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "harbour.screentapshot")
    Q_PROPERTY(int lastXPos READ lastXPos WRITE setLastXPos NOTIFY lastXPosChanged)
    Q_PROPERTY(int lastYPos READ lastYPos WRITE setLastYPos NOTIFY lastYPosChanged)
    Q_PROPERTY(bool screenshotAnimation READ screenshotAnimation WRITE setScreenshotAnimation NOTIFY screenshotAnimationChanged)
    Q_PROPERTY(int screenshotDelay READ screenshotDelay WRITE setScreenshotDelay NOTIFY screenshotDelayChanged)

public:
    explicit ViewHelper(QObject *parent = 0);
    void setDefaultRegion();

    Q_INVOKABLE void closeOverlay();
    Q_INVOKABLE void openStore();

public slots:
    void checkActive();
    void setTouchRegion(const QRect &rect, bool setXY = true);

    Q_SCRIPTABLE Q_NOREPLY void show();
    Q_SCRIPTABLE Q_NOREPLY void exit();

signals:
    void lastXPosChanged();
    void lastYPosChanged();
    void screenshotAnimationChanged();
    void screenshotDelayChanged();

    void applicationRemoval();

private:
    void showOverlay();
    void showSettings();
    void setMouseRegion(const QRegion &region);

    int lastXPos();
    void setLastXPos(int xpos);
    int lastYPos();
    void setLastYPos(int ypos);
    bool screenshotAnimation();
    void setScreenshotAnimation(bool value);
    int screenshotDelay();
    void setScreenshotDelay(int value);

    QQuickView *view;
    MGConfItem *lastXPosConf;
    MGConfItem *lastYPosConf;
    MGConfItem *screenshotAnimationConf;
    MGConfItem *screenshotDelayConf;

private slots:
    void onPackageStatusChanged(const QString &package, int status);

};

#endif // VIEWHELPER_H
