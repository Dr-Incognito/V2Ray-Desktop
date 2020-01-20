import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Layouts 1.3

import com.v2ray.desktop.AppProxy 1.0

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
        columns: 4
        flow: GridLayout.LeftToRight
        rowSpacing: 20
        columnSpacing: 20

        Label {
            text: qsTr("Launch V2Ray Desktop at Login")
            color: "white"
        }

        CheckBox {
            id: checkboxAutoStart
            leftPadding: -3
        }

        Label {
            text: qsTr("Hide Window on Start Up")
            color: "white"
        }

        CheckBox {
            id: checkboxHideWindow
            leftPadding: -3
        }

        Label {
            text: qsTr("Check for Updates Automatically")
            color: "white"
        }

        CheckBox {
            id: checkboxAutoUpdate
            leftPadding: -3
        }

        Label {
            text: qsTr("Enable UDP")
            color: "white"
        }

        CheckBox {
            id: checkboxEnableUdp
            leftPadding: -3
        }

        Label {
            text: qsTr("Local Server Protocol")
            color: "white"
        }

        ComboBox {
            id: comboServerProtocol
            Layout.fillWidth: true
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
            id: textIpAddr
            color: "white"
            Layout.fillWidth: true
            placeholderText: qsTr("Example: 127.0.0.1")
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
            id: textPort
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
            text: qsTr("PAC Server Port")
            color: "white"
        }

        TextField {
            id: textPacServerPort
            color: "white"
            Layout.fillWidth: true
            placeholderText: qsTr("Example: 1085")
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
            Layout.minimumWidth: 180
            Layout.fillWidth: true
            placeholderText: qsTr("Example: 8.8.8.8,8.8.4.4")
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

    AppProxy {
        id: appProxy

        onAppConfigReady: function(config) {
            config = JSON.parse(config)
            checkboxAutoStart.checked = config["autoStart"]
            checkboxHideWindow.checked = config["hideWindow"]
            checkboxAutoUpdate.checked = config["autoUpdate"]
            checkboxEnableUdp.checked = config["enableUdp"]
            comboServerProtocol.currentIndex = comboServerProtocol.find(config["serverProtocol"])
            textIpAddr.text = config["ipAddress"]
            textPort.text = config["port"]
            textPacServerPort.text = config["pacPort"]
            textMux.text = config["mux"]
            textDnsServer.text = config["dns"]
        }
    }

    Component.onCompleted: function() {
        appProxy.getAppConfig()
    }
}
