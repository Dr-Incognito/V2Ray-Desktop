import QtQuick 2.14
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.14
import QtQuick.Dialogs 1.3

import com.v2ray.desktop.AppProxy 1.3

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
            text: Qt.platform.os == "osx" ? qsTr("Preferences") : qsTr("Settings")
            color: "white"
            font.pointSize: Qt.platform.os == "windows" ? 20 : 24
        }
    }

    Label {
        id: labelErrorMsg
        background: Rectangle {
            color: "#ee8989"
        }
        color: "#652424"
        Layout.fillWidth: true
        padding: 10
        visible: false
        wrapMode: Text.Wrap
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
            text: qsTr("Language")
            color: "white"
        }

        ComboBox {
            id: comboLanguage
            Layout.fillWidth: true
            textRole: "text"
            valueRole: "value"
            model: ListModel{
                ListElement { text: "English"; value: "en-US" }
                ListElement { text: "简体中文"; value: "zh-CN" }
            }
            background: Rectangle {
                color: Qt.rgba(255, 255, 255, .1)
                border.color: Qt.rgba(120, 130, 140, .2)
            }
            contentItem: Text {
                text: comboLanguage.displayText
                color: "white"
                leftPadding: 10
                verticalAlignment: Text.AlignVCenter
            }
        }

        Label {
            text: qsTr("Local Server Protocol")
            color: "white"
        }

        ComboBox {
            id: comboServerProtocol
            Layout.fillWidth: true
            model: ListModel{
                ListElement { text: "SOCKS" }
                ListElement { text: "HTTP" }
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
            id: textServerIpAddr
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
            id: textServerPort
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
            text: qsTr("DNS Servers")
            color: "white"
        }

        TextField {
            id: textDnsServers
            color: "white"
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
                var config = {
                    "autoStart": checkboxAutoStart.checked,
                    "hideWindow": checkboxHideWindow.checked,
                    "autoUpdate": checkboxAutoUpdate.checked,
                    "enableUdp": checkboxEnableUdp.checked,
                    "language": comboLanguage.currentValue,
                    "serverProtocol": comboServerProtocol.currentText,
                    "serverIp": textServerIpAddr.text,
                    "serverPort": textServerPort.text,
                    "pacPort": textPacServerPort.text,
                    "dns": textDnsServers.text
                }
                labelErrorMsg.visible = false
                AppProxy.setAppConfig(JSON.stringify(config))
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

    MessageDialog {
        id: messageDialog
        title: qsTr("Message from V2Ray Desktop")
        icon: StandardIcon.Information
        text: qsTr("Settings saved.")
        standardButtons: StandardButton.Ok
    }

    Connections {
        target: AppProxy

        onAppConfigReady: function(config) {
            config = JSON.parse(config)
            checkboxAutoStart.checked = config["autoStart"]
            checkboxHideWindow.checked = config["hideWindow"]
            checkboxAutoUpdate.checked = config["autoUpdate"]
            checkboxEnableUdp.checked = config["enableUdp"]
            comboLanguage.currentIndex = comboLanguage.indexOfValue(config["language"])
            comboServerProtocol.currentIndex = comboServerProtocol.find(config["serverProtocol"])
            textServerIpAddr.text = config["serverIp"]
            textServerPort.text = config["serverPort"]
            textPacServerPort.text = config["pacPort"]
            textDnsServers.text = config["dns"]
        }

        onAppConfigChanged: function() {
            messageDialog.open()
        }

        onAppConfigError: function(errorMsg) {
            labelErrorMsg.text = errorMsg
            labelErrorMsg.visible = true
        }
    }

    Component.onCompleted: function() {
        AppProxy.getAppConfig()
    }
}
