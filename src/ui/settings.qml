import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.platform 1.1

import com.v2ray.desktop.AppProxy 2.4

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
        font.pointSize: 10.5
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
            font.pointSize: 10.5
        }

        CheckBox {
            id: checkboxAutoStart
            leftPadding: -3
        }

        Label {
            text: qsTr("Hide Window on Start Up")
            color: "white"
            font.pointSize: 10.5
        }

        CheckBox {
            id: checkboxHideWindow
            leftPadding: -3
        }

        Label {
            text: qsTr("Language")
            color: "white"
            font.pointSize: 10.5
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
                font.pointSize: 10.5
                leftPadding: 10
                verticalAlignment: Text.AlignVCenter
            }
        }

        Label {
            text: qsTr("Proxy Mode")
            color: "white"
            font.pointSize: 10.5
        }

        ComboBox {
            id: comboProxyMode
            Layout.fillWidth: true
            textRole: "text"
            valueRole: "value"
            model: ListModel{
                ListElement { text: "Rule Mode"; value: "Rule" }
                ListElement { text: "Global Mode"; value: "Global" }
                ListElement { text: "Direct Mode"; value: "Direct" }
            }
            background: Rectangle {
                color: Qt.rgba(255, 255, 255, .1)
                border.color: Qt.rgba(120, 130, 140, .2)
            }
            contentItem: Text {
                text: comboProxyMode.displayText
                color: "white"
                font.pointSize: 10.5
                leftPadding: 10
                verticalAlignment: Text.AlignVCenter
            }
        }

        Label {
            text: qsTr("Listening IP Address")
            color: "white"
            font.pointSize: 10.5
        }

        TextField {
            id: textServerIpAddr
            color: "white"
            font.pointSize: 10.5
            Layout.fillWidth: true
            placeholderText: qsTr("Example: 127.0.0.1")
            background: Rectangle {
                color: Qt.rgba(255, 255, 255, .1)
                border.color: Qt.rgba(120, 130, 140, .2)
            }
        }

        Label {
            text: qsTr("DNS Servers")
            color: "white"
            font.pointSize: 10.5
        }

        TextField {
            id: textDnsServers
            color: "white"
            font.pointSize: 10.5
            Layout.fillWidth: true
            placeholderText: qsTr("Example: 8.8.8.8,8.8.4.4")
            background: Rectangle {
                color: Qt.rgba(255, 255, 255, .1)
                border.color: Qt.rgba(120, 130, 140, .2)
            }
        }

        Label {
            text: qsTr("SOCKS Port")
            color: "white"
            font.pointSize: 10.5
        }

        TextField {
            id: textSocksPort
            color: "white"
            font.pointSize: 10.5
            Layout.fillWidth: true
            placeholderText: qsTr("Example: 1080")
            text: "1080"
            background: Rectangle {
                color: Qt.rgba(255, 255, 255, .1)
                border.color: Qt.rgba(120, 130, 140, .2)
            }
        }

        Label {
            text: qsTr("HTTP Port")
            color: "white"
            font.pointSize: 10.5
        }

        TextField {
            id: textHttpPort
            color: "white"
            font.pointSize: 10.5
            Layout.fillWidth: true
            placeholderText: qsTr("Example: 1087")
            text: "1087"
            background: Rectangle {
                color: Qt.rgba(255, 255, 255, .1)
                border.color: Qt.rgba(120, 130, 140, .2)
            }
        }

        Label {
            text: qsTr("GFW List URL")
            color: "white"
            font.pointSize: 10.5
        }

        TextField {
            id: textGfwListUrl
            color: "white"
            font.pointSize: 10.5
            Layout.fillWidth: true
            Layout.columnSpan: 3
            placeholderText: qsTr("Example: https://url/to/gfwlist.yml")
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
                font.pointSize: 10.5
            }
            background: Rectangle {
                color: parent.enabled ? (parent.down ? "#2980b9" : "#3498db") : "#bdc3c7"
                radius: 4
            }
            onClicked: function() {
                var config = {
                    "autoStart": checkboxAutoStart.checked,
                    "hideWindow": checkboxHideWindow.checked,
                    "language": comboLanguage.currentValue,
                    "serverIp": textServerIpAddr.text,
                    "httpPort": textHttpPort.text,
                    "socksPort": textSocksPort.text,
                    "dns": textDnsServers.text,
                    "gfwListUrl": textGfwListUrl.text
                }
                labelErrorMsg.visible = false
                AppProxy.setProxyMode(comboProxyMode.currentValue)
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
        text: qsTr("Settings saved.")
        buttons: MessageDialog.Ok
    }

    Connections {
        target: AppProxy

        function onAppConfigReady(config) {
            config = JSON.parse(config)
            checkboxAutoStart.checked = config["autoStart"]
            checkboxHideWindow.checked = config["hideWindow"]
            comboLanguage.currentIndex = comboLanguage.indexOfValue(config["language"])
            comboProxyMode.currentIndex = comboProxyMode.indexOfValue(config["proxyMode"])
            textServerIpAddr.text = config["serverIp"]
            textSocksPort.text = config["socksPort"]
            textHttpPort.text = config["httpPort"]
            textDnsServers.text = config["dns"]
            textGfwListUrl.text = config["gfwListUrl"]
        }

        function onV2RayCoreStatusReady(isRunning) {
            comboProxyMode.enabled = isRunning
        }

        function onAppConfigChanged() {
            messageDialog.open()
        }

        function onAppConfigError(errorMsg) {
            labelErrorMsg.text = errorMsg
            labelErrorMsg.visible = true
        }

        function onProxyModeChanged(proxyMode) {
            comboProxyMode.currentIndex = comboProxyMode.indexOfValue(proxyMode)
        }
    }

    Component.onCompleted: function() {
        AppProxy.getAppConfig()
        AppProxy.getV2RayCoreStatus()
    }
}
