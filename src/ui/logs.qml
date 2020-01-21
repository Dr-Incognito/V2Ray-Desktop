import QtQuick 2.0
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

import com.v2ray.desktop.AppProxy 1.0

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
            font.pointSize: 24
        }

        Item {      // spacer item
            Layout.fillWidth: true
            Rectangle {
                anchors.fill: parent
                color: "transparent"
            }
        }

        Button {
            text: qsTr("Clear Logs")
            contentItem: Text {
                text: parent.text
                color: "white"
            }
            background: Rectangle {
                color: parent.enabled ? (parent.down ? "#2980b9" : "#3498db") : "#bdc3c7"
                radius: 4
            }
            onClicked: function() {
                appProxy.clearLogs()
                textLogs.text = ""
            }
        }
    }

    Item {
        Layout.fillWidth: true
        Layout.fillHeight: true
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
                    if (contentX >= r.x)
                        contentX = r.x;
                    else if (contentX+width <= r.x+r.width)
                        contentX = r.x+r.width-width;
                    if (contentY >= r.y)
                        contentY = r.y;
                    else if (contentY+height <= r.y+r.height)
                        contentY = r.y+r.height-height;
                }

                TextEdit {
                    id: textLogs
                    color: "white"
                    focus: true
                    readOnly: true
                    selectByMouse: true
                    textMargin: 10
                    wrapMode: Text.Wrap
                    width: flick.width
                    onCursorRectangleChanged: flick.ensureVisible(cursorRectangle)
                }
            }
        }
    }

    AppProxy {
        id: appProxy

        onLogsReady: function(logs) {
            textLogs.text = logs
        }
    }

    Timer {
        id: timer
        interval: 2500
        running: true
        repeat: true
        onTriggered: function() {
            appProxy.getLogs()
        }
    }
}
