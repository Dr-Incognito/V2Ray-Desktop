import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import com.v2ray.desktop.AppProxy 2.4

ColumnLayout {
    anchors.fill: parent
    anchors.margins: 10
    spacing: 20

    RowLayout {
        Image {
            source: "qrc:///images/icon-logs.svg"
            sourceSize.width: 40
            sourceSize.height: 40
        }

        Text {
            text: qsTr("Logs")
            color: "white"
            font.pointSize: Qt.platform.os == "windows" ? 20 : 24
        }

        Item {      // spacer item
            Layout.fillWidth: true

            Rectangle {
                anchors.fill: parent
                color: "transparent"
            }
        }

        Button {
            /* Fix the layout bug introduced in Qt 5.15 */
            Layout.rightMargin: 20

            text: qsTr("Clear Logs")
            contentItem: Text {
                text: parent.text
                color: "white"
                font.pointSize: 10.5
                padding: 4
            }
            background: Rectangle {
                color: parent.enabled ? (parent.down ? "#c0392b" : "#e74c3c") : "#bdc3c7"
                radius: 4
            }
            onClicked: function() {
                AppProxy.clearLogs()
                textLogs.text = ""
            }
        }
    }

    Item {
        Layout.fillWidth: true
        Layout.fillHeight: true
        /* Fix the layout bug introduced in Qt 5.15 */
        Layout.rightMargin: 20

        Rectangle {
            id: rectLogs
            width:parent.width
            height:parent.height
            border.color: "#fff"
            color: "#2e3e4e"

            Flickable {
                id: flick
                anchors.fill: parent
                contentWidth: textLogs.paintedWidth
                contentHeight: textLogs.paintedHeight
                clip: true

                function ensureVisible(r) {
                    if (contentX >= r.x) {
                        contentX = r.x;
                    } else if (contentX+width <= r.x+r.width) {
                        contentX = r.x+r.width-width;
                    } if (contentY >= r.y) {
                        contentY = r.y;
                    } else if (contentY+height <= r.y+r.height) {
                        contentY = r.y+r.height-height;
                    }
                }

                TextEdit {
                    id: textLogs
                    color: "white"
                    focus: true
                    readOnly: true
                    selectByMouse: true
                    text: qsTr("Loading logs ...")
                    font.pointSize: 10.5
                    textMargin: 10
                    wrapMode: Text.Wrap
                    width: flick.width
                    onCursorRectangleChanged: flick.ensureVisible(cursorRectangle)
                }
            }
        }
    }

    Connections {
        target: AppProxy

        function onLogsReady(logs) {
            textLogs.text = logs
        }
    }

    Timer {
        interval: 5000
        running: true
        repeat: true
        onTriggered: function() {
            AppProxy.getLogs()
        }
    }

    Component.onCompleted: function() {
        AppProxy.getLogs()
    }
}
