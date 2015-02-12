#include <QtGui/QGuiApplication>
#include <QtQml>
#include <QQuickView>
#include <QQmlEngine>
#include <QQmlContext>
#include <qpa/qplatformnativeinterface.h>
#include <QScopedPointer>
#include <QTimer>

#include <sailfishapp.h>

#include "viewhelper.h"
#include "screenshot.h"

int main(int argc, char *argv[])
{
    QScopedPointer<QGuiApplication> application(SailfishApp::application(argc, argv));
    QScopedPointer<QQuickView> view(SailfishApp::createView());
    QObject::connect(view->engine(), SIGNAL(quit()), application.data(), SLOT(quit()));
    QScopedPointer<ViewHelper> helper(new ViewHelper(view.data()));
    view->rootContext()->setContextProperty("viewHelper", helper.data());

    qmlRegisterType<Screenshot>("harbour.coderus.screentapshot", 1, 0, "Screenshot");

    QColor color;
    color.setRedF(0.0);
    color.setGreenF(0.0);
    color.setBlueF(0.0);
    color.setAlphaF(0.0);
    view->setColor(color);

    view->setClearBeforeRendering(true);

    view->setSource(SailfishApp::pathTo("qml/main.qml"));

    view->showFullScreen();

    QTimer::singleShot(1, helper.data(), SLOT(detachWindow()));

    return application->exec();
}
