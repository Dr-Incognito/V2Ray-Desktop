import QtQuick 2.0
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
                anchors.fill: parent
                flickableDirection: Flickable.VerticalFlick
                clip: true
                TextEdit {
                    id: textLogs
                    width: rectLogs.width
                    wrapMode: TextEdit.Wrap
                    anchors.fill: parent
                    color: "white"
                    textMargin: 10
                    readOnly: true
                    selectByMouse: true
                    text: ""
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

    Component.onCompleted: function() {
        appProxy.getLogs()
    }
}
