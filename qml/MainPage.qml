import QtQuick 2.1
import Sailfish.Silica 1.0

Page {
    id: page
    objectName: "mainPage"

    SilicaFlickable {
        id: flick
        anchors.fill: page
        contentHeight: content.height

        PullDownMenu {
            MenuItem {
                text: qsTr("Close overlay")
                onClicked: viewHelper.closeOverlay()
            }
            MenuItem {
                text: qsTr("About")
                onClicked: pageStack.push(Qt.resolvedUrl("AboutPage.qml"))
            }
        }

        Column {
            id: content
            width: parent.width

            PageHeader {
                title: qsTr("ScreenTapShot")
            }

            TextSwitch {
                text: qsTr("Screenshot animation")
                checked: viewHelper.screenshotAnimation
                onClicked: viewHelper.screenshotAnimation = checked
            }

            Slider {
                width: parent.width
                label: qsTr("Screenshot delay")
                minimumValue: 0
                maximumValue: 10
                value: viewHelper.screenshotDelay
                valueText: parseInt(value) == 0 ? qsTr("No delay") : qsTr("%1s").arg(parseInt(value))
                onReleased: viewHelper.screenshotDelay = parseInt(value)
            }

            Label {
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: Theme.paddingLarge
                }
                text: viewHelper.screenshotDelay == 0
                      ? qsTr("With no delay you can close overlay only using settings page pulldown menu.")
                      : qsTr("When delay is set you can close overlay with doubletap on capture icon")
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.secondaryColor
            }

            TextSwitch {
                text: qsTr("Save in Screenshots subfolder")
                checked: viewHelper.useSubfolder
                onClicked: viewHelper.useSubfolder = checked
            }
        }
    }
}
