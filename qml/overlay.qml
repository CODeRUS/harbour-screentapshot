import QtQuick 2.1
import Sailfish.Silica 1.0
import QtSensors 5.0
import QtGraphicalEffects 1.0
import harbour.screentapshot2.screenshot 1.0

Item {
    id: root

    width: Screen.width
    height: Screen.height

    property string icon: "capture"

    Connections {
        target: viewHelper
        onApplicationRemoval: {
            removalOverlay.opacity = 1.0
        }
    }

    OrientationSensor {
        id: rotationSensor
        active: true
        property bool hack: if (reading.orientation) _getOrientation(reading.orientation)
        property int sensorAngle: 0
        property int angle: viewHelper.orientationLock == "dynamic" || viewHelper.orientationLock == ""
                            ? sensorAngle
                            : (viewHelper.orientationLock == "portrait" ? 0 : 90)
        function _getOrientation(value) {
            switch (value) {
            case 1:
                sensorAngle = 0
                break
            case 2:
                sensorAngle = 180
                break
            case 3:
                sensorAngle = -90
                break
            case 4:
                sensorAngle = 90
                break
            default:
                return false
            }
            return true
        }
    }

    Screenshot {
        id: screenshot
        useSubfolder: viewHelper.useSubfolder
        onCaptured: {
            shotPreview.width = root.width
            shotPreview.height = root.height
            shotPreview.source = path
            restoreTimer.start()
        }
    }

    Timer {
        id: delayTimer
        interval: viewHelper.screenshotDelay * 10
        repeat: true
        onTriggered: {
            captureProgress.value += 0.01
            if (captureProgress.value >= 1.0) {
                delayTimer.stop()
                captureProgress.value = 0.0
                root.visible = false
                touchArea.count = 0
                captureTimer.start()
            }
        }
    }

    Timer {
        id: captureTimer
        interval: 100
        onTriggered: {
            screenshot.capture()
        }
    }

    Timer {
        id: restoreTimer
        interval: 10
        onTriggered: {
            if (viewHelper.screenshotAnimation) {
                shotPreview.opacity = 1.0
                root.visible = true
                animate.start()
            }
            else {
                root.visible = true
            }
        }
    }

    MouseArea {
        id: touchArea

        x: viewHelper.lastXPos
        y: viewHelper.lastYPos

        width: Theme.itemSizeSmall
        height: Theme.itemSizeSmall

        drag.target: touchArea
        drag.minimumX: 0
        drag.maximumX: root.width - touchArea.width
        drag.minimumY: 0
        drag.maximumY: root.height - touchArea.height

        property int count: 0
        onCountChanged: {
            if (count > 6) {
                count = 0
                iconItem.deltaAngle += 90
            }
            if (iconItem.deltaAngle == 720) {
                if (root.icon == "sailfish") {
                    root.icon = "capture"
                }
                else {
                    root.icon = "sailfish"
                }
                iconItem.deltaAngle = 0
            }
        }

        function capture() {
            count += 1
            if (delayTimer.running) {
                delayTimer.stop()
                captureProgress.value = 0.0
            }
            else {
                if (viewHelper.screenshotDelay > 0) {
                    delayTimer.start()
                }
                else {
                    root.visible = false
                    touchArea.count = 0
                    captureTimer.start()
                }
            }
        }

        onDoubleClicked: {
            Qt.quit()
        }
        onClicked: {
            capture()
        }
        onReleased: viewHelper.setTouchRegion(Qt.rect(x, y, width, height))

        Item {
            id: iconItem
            anchors.fill: parent
            property int deltaAngle: 0
            rotation: rotationSensor.angle + deltaAngle
            Behavior on rotation {
                SmoothedAnimation { duration: 500 }
            }
            Rectangle {
                anchors.fill: parent
                radius: Math.max(width, height) / 2
                color: Theme.rgba(Theme.highlightBackgroundColor, Theme.highlightBackgroundOpacity)
            }
            Image {
                id: highlightedIcon
                anchors.centerIn: parent
                smooth: true
                source: "image://theme/icon-m-camera"
            }
        }

        ProgressCircle {
            id: captureProgress

            anchors.fill: parent
            value: 0.0
            visible: delayTimer.running
        }
    }

    Image {
        id: shotPreview

        anchors.centerIn: parent

        opacity: 0.0

        PropertyAnimation {
            id: animate
            target: shotPreview
            properties: "height,width,opacity"
            to: 0
            duration: 500
        }
    }

    MouseArea {
        id: removalOverlay

        anchors.fill: parent
        enabled: opacity == 1.0
        onEnabledChanged: {
            if (enabled)
                viewHelper.setTouchRegion(Qt.rect(0, 0, Screen.width, Screen.height), false)
        }
        opacity: 0.0
        Behavior on opacity {
            SmoothedAnimation { duration: 1000 }
        }

        onClicked: {
            Qt.quit()
        }

        MouseArea {
            anchors {
                fill: removalContent
                margins: -Theme.paddingLarge
            }
            enabled: removalOverlay.enabled

            Rectangle {
                anchors.fill: parent
                color: Theme.highlightDimmerColor
            }
        }

        Column {
            id: removalContent

            anchors {
                centerIn: parent
            }
            width: Screen.width - Theme.paddingLarge * 2

            spacing: Theme.paddingLarge

            Row {
                spacing: Theme.paddingLarge
                height: iconContent.height
                anchors.horizontalCenter: parent.horizontalCenter

                Label {
                    anchors.verticalCenter: parent.verticalCenter
                    //: Title of overlay visible while removing still launched application
                    text: qsTr("Application removal")
                }

                Item {
                    id: iconContent

                    width: appIcon.sourceSize.width
                    height: appIcon.sourceSize.height

                    Image {
                        id: appIcon
                        anchors.centerIn: parent
                        source: "image://theme//harbour-screentapshot2"
                    }

                    Label {
                        id: sadFace
                        anchors.centerIn: parent
                        text: ":("
                        font.bold: true
                        opacity: 0.0
                    }

                    Timer {
                        interval: 3000
                        running: removalOverlay.enabled
                        repeat: true
                        onTriggered: {
                            if (iconContent.rotation == 0) {
                                sadAnimation.start()
                            }
                            else {
                                iconAnimation.start()
                            }
                        }
                    }

                    ParallelAnimation {
                        id: sadAnimation
                        NumberAnimation {
                            target: iconContent
                            property: "rotation"
                            from: 0
                            to: 360
                            duration: 1000
                        }
                        NumberAnimation {
                            target: appIcon
                            property: "opacity"
                            from: 1.0
                            to: 0.0
                            duration: 1000
                        }
                        NumberAnimation {
                            target: sadFace
                            property: "opacity"
                            from: 0.0
                            to: 1.0
                            duration: 1000
                        }
                    }

                    ParallelAnimation {
                        id: iconAnimation
                        NumberAnimation {
                            target: iconContent
                            property: "rotation"
                            from: 360
                            to: 0
                            duration: 1000
                        }
                        NumberAnimation {
                            target: appIcon
                            property: "opacity"
                            from: 0.0
                            to: 1.0
                            duration: 1000
                        }
                        NumberAnimation {
                            target: sadFace
                            property: "opacity"
                            from: 1.0
                            to: 0.0
                            duration: 1000
                        }
                    }
                }
            }

            Label {
                width: parent.width
                wrapMode: Text.Wrap
                horizontalAlignment: Text.AlignHCenter
                //: Removal overlay text
                text: qsTr("I'm sorry You unsatisfied with my application. Please tell me why, and I will try to do my best to improve it.")
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                //: Removal overlay button to open openrepos in browser
                text: qsTr("Leave comment in OpenRepos")
                enabled: removalOverlay.enabled
                onClicked: {
//                    viewHelper.openStore()
                    Qt.openUrlExternally("https://openrepos.net/content/coderus/screentapshot2")
                    Qt.quit()
                }
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                //: Removal overlay button to close application
                text: qsTr("No, thanks")
                enabled: removalOverlay.enabled
                onClicked: Qt.quit()
            }
        }
    }
}
