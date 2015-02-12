#ifndef VIEWHELPER_H
#define VIEWHELPER_H

#include <QObject>
#include <QQuickView>
#include <mlite5/MGConfItem>

class ViewHelper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int lastXPos READ lastXPos WRITE setLastXPos NOTIFY lastXPosChanged)
    Q_PROPERTY(int lastYPos READ lastYPos WRITE setLastYPos NOTIFY lastYPosChanged)

public:
    explicit ViewHelper(QQuickView *parent = 0);
    void setDefaultRegion();

public slots:
    void setTouchRegion(const QRect &rect);
    void detachWindow();

signals:
    void lastXPosChanged();
    void lastYPosChanged();

private:
    void setMouseRegion(const QRegion &region);

    int lastXPos();
    void setLastXPos(int xpos);
    int lastYPos();
    void setLastYPos(int ypos);

    QQuickView *view;
    MGConfItem *lastXPosConf;
    MGConfItem *lastYPosConf;

};

#endif // VIEWHELPER_H
