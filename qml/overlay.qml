import QtQuick 2.1
import Sailfish.Silica 1.0
import QtSensors 5.0
import harbour.screentapshot.screenshot 1.0

Item {
    id: root

    width: Screen.width
    height: Screen.height

    property string icon: "capture"

    OrientationSensor {
        id: rotationSensor
        active: true
        property int angle: reading.orientation ? _getOrientation(reading.orientation) : 0
        function _getOrientation(value) {
            switch (value) {

            case 2:
                return 180
            case 3:
                return -90
            case 4:
                return 90
            default:
                return 0
            }
        }
    }

    Screenshot {
        id: screenshot
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

        width: 80
        height: 80

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

        onDoubleClicked: Qt.quit()
        onClicked: {
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
        onReleased: viewHelper.setTouchRegion(Qt.rect(x, y, width, height))

        Item {
            id: iconItem
            anchors.fill: parent
            property int deltaAngle: 0
            rotation: rotationSensor.angle + deltaAngle
            Behavior on rotation {
                SmoothedAnimation { duration: 500 }
            }
            Image {
                id: mainIcon
                anchors.centerIn: parent
                source: "../images/" + root.icon + "-blur.png"
                property color color: Theme.rgba("black", Theme.highlightBackgroundOpacity)
                layer.effect: ShaderEffect {
                    property color color: mainIcon.color

                    fragmentShader: "
                        varying mediump vec2 qt_TexCoord0;
                        uniform highp float qt_Opacity;
                        uniform lowp sampler2D source;
                        uniform highp vec4 color;
                        void main() {
                            highp vec4 pixelColor = texture2D(source, qt_TexCoord0);
                            gl_FragColor = vec4(mix(pixelColor.rgb/max(pixelColor.a, 0.00390625), color.rgb/max(color.a, 0.00390625), color.a) * pixelColor.a, pixelColor.a) * qt_Opacity;
                        }
                    "
                }
                layer.enabled: true
                layer.samplerName: "source"
            }
            Image {
                id: highlightedIcon
                anchors.centerIn: parent
                source: "../images/" + root.icon + ".png"
                property color color: Theme.highlightColor
                layer.effect: ShaderEffect {
                    property color color: highlightedIcon.color

                    fragmentShader: "
                        varying mediump vec2 qt_TexCoord0;
                        uniform highp float qt_Opacity;
                        uniform lowp sampler2D source;
                        uniform highp vec4 color;
                        void main() {
                            highp vec4 pixelColor = texture2D(source, qt_TexCoord0);
                            gl_FragColor = vec4(mix(pixelColor.rgb/max(pixelColor.a, 0.00390625), color.rgb/max(color.a, 0.00390625), color.a) * pixelColor.a, pixelColor.a) * qt_Opacity;
                        }
                    "
                }
                layer.enabled: true
                layer.samplerName: "source"
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

        anchors.centerIn: root

        opacity: 0.0

        PropertyAnimation {
            id: animate
            target: shotPreview
            properties: "height,width,opacity"
            to: 0
            duration: 500
        }
    }
}
