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
                text: "Close overlay"
                onClicked: viewHelper.closeOverlay()
            }
            MenuItem {
                text: "About"
                onClicked: pageStack.push(Qt.resolvedUrl("AboutPage.qml"))
            }
        }

        Column {
            id: content
            width: parent.width

            PageHeader {
                title: "ScreenTapShot"
            }

            TextSwitch {
                text: "Screenshot animation"
                checked: viewHelper.screenshotAnimation
                onClicked: viewHelper.screenshotAnimation = checked
            }

            Slider {
                width: parent.width
                label: "Screenshot delay"
                minimumValue: 0
                maximumValue: 10
                value: viewHelper.screenshotDelay
                valueText: parseInt(value) == 0 ? "No delay" : (parseInt(value) + "s")
                onReleased: viewHelper.screenshotDelay = parseInt(value)
            }

            Label {
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: Theme.paddingLarge
                }
                text: viewHelper.screenshotDelay == 0
                      ? "With no delay you can close overlay only using settings page pulldown menu."
                      : "When delay is set you can close overlay with doubletap on capture icon"
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.secondaryColor
            }

            TextSwitch {
                text: "Save in Screehshots subfolder"
                checked: viewHelper.useSubfolder
                onClicked: viewHelper.useSubfolder = checked
            }
        }
    }
}
