import QtQuick 2.1
import Sailfish.Silica 1.0

ApplicationWindow
{
    _defaultPageOrientations: Orientation.All
    _allowedOrientations: Orientation.All
    initialPage: Component { MainPage { } }
    cover: Qt.resolvedUrl("CoverPage.qml")
}


