import QtQuick 2.1
import Sailfish.Silica 1.0

ApplicationWindow
{
    _defaultPageOrientations: Orientation.Portrait
    _allowedOrientations: Orientation.Portrait
    onApplicationActiveChanged: {
        if (applicationActive) {
            view.showFullScreen()
            dummyView.close()
            //closeLater.start()
        }
    }

    Timer {
        id: closeLater
        repeat: false
        interval: 1
        onTriggered: dummyView.close()
    }

    initialPage: Page {
        Label {
            anchors.fill: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: "Screen\nTap\nShot\nSettings"
            font.pixelSize: parent.height / 11
            color: Theme.secondaryColor
        }
    }
    cover: Qt.resolvedUrl("CoverPage.qml")
}


