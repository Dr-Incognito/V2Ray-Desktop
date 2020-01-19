import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Layouts 1.3


ColumnLayout {
    anchors.fill: parent
    anchors.margins: 10
    spacing: 20

    RowLayout {
        Image {
            source: "qrc:///images/icon-settings.svg"
            sourceSize.width: 40
            sourceSize.height: 40
        }

        Text {
            text: qsTr("Settings")
            color: "white"
            font.pointSize: 24
        }
    }

    GridLayout {
        columns: 2
        flow: GridLayout.LeftToRight
        rowSpacing: 20
        columnSpacing: 20

        Label {
            text: qsTr("Launch V2Ray Desktop at Login")
            color: "white"
        }

        CheckBox {
            checked: true
            leftPadding: -3
        }

        Label {
            text: qsTr("Check for Updates Automatically")
            color: "white"
        }

        CheckBox {
            checked: true
            leftPadding: -3
        }

        Label {
            text: qsTr("Local Server Protocol")
            color: "white"
        }

        ComboBox {
            id: comboServerProtocol
            model: ListModel{
                ListElement { text: "SOCKS 5" }
                ListElement { text: "HTTP/HTTPS" }
            }
            background: Rectangle {
                color: Qt.rgba(255, 255, 255, .1)
                border.color: Qt.rgba(120, 130, 140, .2)
            }
            contentItem: Text {
                text: comboServerProtocol.displayText
                color: "white"
                leftPadding: 10
                verticalAlignment: Text.AlignVCenter
            }
        }

        Label {
            text: qsTr("Listening IP Address")
            color: "white"
        }

        TextField {
            id: textSocks5IpAddr
            color: "white"
            Layout.fillWidth: true
            placeholderText: qsTr("Example: 127.0.0.1")
            text: "127.0.0.1"
            background: Rectangle {
                color: Qt.rgba(255, 255, 255, .1)
                border.color: Qt.rgba(120, 130, 140, .2)
            }
        }

        Label {
            text: qsTr("Listening Port")
            color: "white"
        }

        TextField {
            id: textSocks5Port
            color: "white"
            Layout.fillWidth: true
            placeholderText: qsTr("Example: 1080")
            text: "1080"
            background: Rectangle {
                color: Qt.rgba(255, 255, 255, .1)
                border.color: Qt.rgba(120, 130, 140, .2)
            }
        }

        Label {
            text: qsTr("MUX")
            color: "white"
        }

        TextField {
            id: textMux
            color: "white"
            Layout.fillWidth: true
            placeholderText: qsTr("Example: 8 (-1 for disabled)")
            text: "-1"
            background: Rectangle {
                color: Qt.rgba(255, 255, 255, .1)
                border.color: Qt.rgba(120, 130, 140, .2)
            }
        }

        Label {
            text: qsTr("DNS Server")
            color: "white"
        }

        TextField {
            id: textDnsServer
            color: "white"
            Layout.fillWidth: true
            placeholderText: qsTr("Example: 8.8.8.8,8.8.4.4")
            text: "8.8.8.8,8.8.4.4"
            background: Rectangle {
                color: Qt.rgba(255, 255, 255, .1)
                border.color: Qt.rgba(120, 130, 140, .2)
            }
        }

        Button {
            id: buttonSaveSettings
            text: qsTr("Save Settings")
            contentItem: Text {
                text: parent.text
                color: "white"
            }
            background: Rectangle {
                color: parent.enabled ? (parent.down ? "#2980b9" : "#3498db") : "#bdc3c7"
                radius: 4
            }
            onClicked: function() {
            }
        }
    }

    Item {      // spacer item
        Layout.fillWidth: true
        Layout.fillHeight: true
        Rectangle {
            anchors.fill: parent
            color: "transparent"
        }
    }
}
