#include <QtGui/QGuiApplication>
#include <QScopedPointer>
#include <QTimer>
#include <sailfishapp.h>

#include "viewhelper.h"

int main(int argc, char *argv[])
{
    QScopedPointer<QGuiApplication> application(SailfishApp::application(argc, argv));
    application->setApplicationVersion(QString(APP_VERSION));
    QScopedPointer<ViewHelper> helper(new ViewHelper(application.data()));

    QTimer::singleShot(1, helper.data(), SLOT(checkActive()));

    return application->exec();
}
