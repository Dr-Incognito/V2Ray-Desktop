import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.4
import QtQuick.Dialogs 1.3

import com.v2ray.desktop.AppProxy 2.2

ColumnLayout {
    id: layoutServer
    anchors.fill: parent
    anchors.margins: 10
    spacing: 20

    RowLayout {
        Image {
            source: "qrc:///images/icon-servers.svg"
            sourceSize.width: 40
            sourceSize.height: 40
        }

        Text {
            text: qsTr("Servers")
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
            text: qsTr("Server Subscriptions")
            contentItem: Text {
                text: parent.text
                color: "white"
            }
            background: Rectangle {
                color: parent.enabled ? (parent.down ? "#8e44ad" : "#9b59b6") : "#bdc3c7"
                radius: 4
            }
            onClicked: function() {
                labelSubscriptionErrorMsg.visible = false
                popUpSubscription.open()
            }
        }

        Button {
            /* Fix the layout bug introduced in Qt 5.15 */
            Layout.rightMargin: 20

            text: qsTr("Add New Servers")
            contentItem: Text {
                text: parent.text
                color: "white"
            }
            background: Rectangle {
                color: parent.enabled ? (parent.down ? "#2980b9" : "#3498db") : "#bdc3c7"
                radius: 4
            }
            onClicked: function() {
                layoutServer.resetPopUpServerFields("add")
                popUpServer.open()
            }
        }
    }

    Item {
        Layout.fillWidth: true
        Layout.fillHeight: true
        /* Fix the layout bug introduced in Qt 5.15 */
        Layout.rightMargin: 20

        ListView {
            id: listViewServers
            anchors.fill: parent
            anchors.rightMargin: 5
            flickableDirection: Flickable.HorizontalAndVerticalFlick
            headerPositioning: ListView.OverlayHeader
            clip: true

            function getColumnWidth(index) {
                switch (index) {
                case 0:
                    return listViewServers.width * 0.25
                case 1:
                    return listViewServers.width * 0.275
                case 2:
                    return listViewServers.width * 0.175
                case 3:
                    return listViewServers.width * 0.175
                case 4:
                    return listViewServers.width * 0.125
                default:
                    return 0
                }
            }

            header: Row {
                spacing: 1
                z: 4

                function itemAt(index) {
                    return listViewServersRepeater.itemAt(index)
                }
                Repeater {
                    id: listViewServersRepeater
                    model: [
                        qsTr("Name"), qsTr("Server"), qsTr("Protocol"), qsTr("Status"),
                        qsTr("Latency")
                    ]
                    Label {
                        text: modelData
                        color: "white"
                        font.bold: true
                        padding: 10
                        width: listViewServers.getColumnWidth(index)
                        background: Rectangle {
                            color: "#354759"
                        }
                    }
                }
            }

            model: listModelServers
            delegate: Column {
                Row {
                    spacing: 1
                    Repeater {
                        model: values
                        ItemDelegate {
                            text: value
                            width: listViewServers.getColumnWidth(index)

                            contentItem: Text {
                                clip: true
                                color: "white"
                                text: parent.text
                            }
                            background: MouseArea {
                                anchors.fill: parent
                                acceptedButtons: Qt.LeftButton | Qt.RightButton
                                onClicked: function(mouse) {
                                    if (mouse.button !== Qt.RightButton) {
                                        return
                                    }
                                    var serverName = parent.parent.data[0].text,
                                    isConnected = parent.parent.data[3].text === qsTr("Connected")

                                    menuServer.x = parent.x + mouseX
                                    menuServer.y = parent.y + mouseY
                                    menuItemServerName.text = serverName
                                    menuItemConnect.text = isConnected ? qsTr("Disconnect") : qsTr("Connect")
                                    menuServer.open()
                                }
                                onDoubleClicked: function(mouse) {
                                    if (mouse.button !== Qt.LeftButton) {
                                        return
                                    }
                                    var serverName = parent.parent.data[0].text,
                                    isConnected = parent.parent.data[3].text === qsTr("Connected")

                                    AppProxy.setServerConnection(serverName, !isConnected)
                                }
                            }
                        }
                    }
                }
                Rectangle {
                    color: "#3b4d5d"
                    width: parent.width
                    height: 1
                }
            }

            ListModel {
                id: listModelServers
            }

            ScrollIndicator.horizontal: ScrollIndicator { }
            ScrollIndicator.vertical: ScrollIndicator { }
        }

        Menu {
            id: menuServer
            padding: 5

            MenuItem {
                id: menuItemServerName
                enabled: false
            }

            MenuSeparator { }

            MenuItem {
                id: menuItemConnect
                text: qsTr("Connect")
                onTriggered: function() {
                    var serverName = menuItemServerName.text,
                    connected = menuItemConnect.text === qsTr("Connect")

                    AppProxy.setServerConnection(serverName, connected)
                }
            }

            MenuItem {
                id: menuItemTestLatency
                text: qsTr("Test Latency")
                onTriggered: function() {
                    menuItemTestLatency.enabled = false
                    menuItemTestAllLatency.enabled = false
                    AppProxy.getServerLatency(menuItemServerName.text)
                }
            }

            MenuItem {
                id: menuItemTestAllLatency
                text: qsTr("Test All Latency")
                onTriggered: function() {
                    menuItemTestLatency.enabled = false
                    menuItemTestAllLatency.enabled = false
                    AppProxy.getServerLatency()
                }
            }

            MenuSeparator { }

            MenuItem {
                id: menuItemEdit
                text: qsTr("Edit")
                onTriggered: function() {
                    AppProxy.getServer(menuItemServerName.text, false)
                }
            }

            MenuItem {
                id: menuItemDuplicate
                text: qsTr("Duplicate")
                onTriggered: function() {
                    AppProxy.getServer(menuItemServerName.text, true)
                }
            }

            MenuItem {
                id: menuItemDelete
                text: qsTr("Delete")
                onTriggered: function() {
                    AppProxy.removeServer(menuItemServerName.text)
                }
            }
        }
    }

    Popup {
        id: popUpServer
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        height: parent.height
        focus: true
        modal: true
        width: parent.width
        background: Rectangle {
            color: "#2e3e4e"
        }
        // The variable is used for saving the old name of server before editing
        property string editServerName

        ScrollView {
            anchors.fill: parent
            contentHeight: colLayoutServer.height
            contentWidth: colLayoutServer.width
            clip: true
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

            ColumnLayout {
                id: colLayoutServer
                anchors.margins: 10
                width: popUpServer.width - 25
                spacing: 20

                RowLayout {
                    Label {
                        id: labelServerInformation
                        text: qsTr("Server Information")
                        color: "white"
                        font.pixelSize: 20
                    }

                    Label {
                        id: labelAddServerMethod
                        text: qsTr("Add new servers by ")
                        color: "white"
                    }

                    ComboBox {
                        id: comboAddServerMethod
                        Layout.fillWidth: true
                        textRole: "text"
                        valueRole: "value"
                        model: ListModel{
                            ListElement {
                                text: qsTr("Manually setting up a V2Ray server");
                                value: "v2ray-manual"
                            }
                            ListElement {
                                text: qsTr("Manually setting up a Shadowsocks(R) server");
                                value: "shadowsocks-manual"
                            }
                            ListElement {
                                text: qsTr("Manually setting up a Trojan server");
                                value: "trojan-manual"
                            }
                            ListElement {
                                text: qsTr("Subscription URL");
                                value: "subscription"
                            }
                            ListElement {
                                text: qsTr("V2Ray config files");
                                value: "v2ray-config"
                            }
                            ListElement {
                                text: qsTr("Shadowsocks-Qt5 config files (gui-config.json)");
                                value: "shadowsocks-qt5-config"
                            }
                        }
                        background: Rectangle {
                            color: Qt.rgba(255, 255, 255, .1)
                            border.color: Qt.rgba(120, 130, 140, .2)
                        }
                        contentItem: Text {
                            text: comboAddServerMethod.displayText
                            color: "white"
                            leftPadding: 10
                            verticalAlignment: Text.AlignVCenter
                        }
                        onCurrentValueChanged: function() {
                            layoutServerV2rayManually.visible = false
                            layoutServerShadowsocksManually.visible = false
                            layoutServerTrojanManually.visible = false
                            layoutServerSubscriptionUrl.visible = false
                            layoutServerJsonFiles.visible = false
                            labelServerConfigErrorMsg.visible = false

                            if ( comboAddServerMethod.currentValue === "v2ray-manual" ) {
                                layoutServerV2rayManually.visible = true
                            } else if ( comboAddServerMethod.currentValue === "shadowsocks-manual" ) {
                                layoutServerShadowsocksManually.visible = true
                            } else if ( comboAddServerMethod.currentValue === "trojan-manual" ) {
                                layoutServerTrojanManually.visible = true
                            } else if ( comboAddServerMethod.currentValue === "subscription" ) {
                                layoutServerSubscriptionUrl.visible = true
                            } else {
                                layoutServerJsonFiles.visible = true
                            }
                        }
                    }
                }

                Label {
                    id: labelServerConfigErrorMsg
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
                    id: layoutServerV2rayManually
                    columns: 4
                    flow: GridLayout.LeftToRight
                    columnSpacing: 20
                    rowSpacing: 20
                    width: parent.width

                    Label {
                        id: labelShadowsocksServerName
                        text: qsTr("Server Name")
                        color: "white"
                    }

                    TextField {
                        id: textV2RayServerName
                        color: "white"
                        Layout.fillWidth: true
                        placeholderText: qsTr("Example: HongKong-Server-1")
                        background: Rectangle {
                            color: Qt.rgba(255, 255, 255, .1)
                            border.color: Qt.rgba(120, 130, 140, .2)
                        }
                    }

                    Label {
                        text: qsTr("Auto Connect")
                        color: "white"
                    }

                    CheckBox {
                        id: checkboxV2RayAutoConnect
                        leftPadding: -2
                    }

                    Label {
                        text: qsTr("Server Address")
                        color: "white"
                    }

                    TextField {
                        id: textV2RayServerAddr
                        color: "white"
                        Layout.fillWidth: true
                        placeholderText: qsTr("Example: hk.example.com")
                        background: Rectangle {
                            color: Qt.rgba(255, 255, 255, .1)
                            border.color: Qt.rgba(120, 130, 140, .2)
                        }
                    }

                    Label {
                        text: qsTr("Server Port")
                        color: "white"
                    }

                    TextField {
                        id: textV2RayServerPort
                        color: "white"
                        Layout.fillWidth: true
                        placeholderText: qsTr("Example: 443")
                        background: Rectangle {
                            color: Qt.rgba(255, 255, 255, .1)
                            border.color: Qt.rgba(120, 130, 140, .2)
                        }
                    }

                    Label {
                        text: qsTr("ID")
                        color: "white"
                    }

                    TextField {
                        id: textV2RayId
                        color: "white"
                        Layout.minimumWidth: 200
                        Layout.fillWidth: true
                        placeholderText: qsTr("Example: 27848739-7e62-4138-9fd3-098a63964b6b")
                        background: Rectangle {
                            color: Qt.rgba(255, 255, 255, .1)
                            border.color: Qt.rgba(120, 130, 140, .2)
                        }
                    }

                    Label {
                        text: qsTr("Alter ID")
                        color: "white"
                    }

                    TextField {
                        id: textV2RayAlterId
                        color: "white"
                        Layout.fillWidth: true
                        placeholderText: qsTr("Example: 4")
                        background: Rectangle {
                            color: Qt.rgba(255, 255, 255, .1)
                            border.color: Qt.rgba(120, 130, 140, .2)
                        }
                    }

                    Label {
                        text: qsTr("Security")
                        color: "white"
                    }

                    ComboBox {
                        id: comboV2RaySecurity
                        Layout.minimumWidth: 180
                        Layout.fillWidth: true
                        textRole: "text"
                        valueRole: "value"
                        model: ListModel{
                            ListElement { text: "Auto"; value: "auto" }
                            ListElement { text: "None"; value: "none" }
                            ListElement { text: "AES-128-GCM"; value: "aes-128-gcm" }
                            ListElement { text: "CHACHA20-POLY1305"; value: "chacha20-poly1305" }
                        }
                        background: Rectangle {
                            color: Qt.rgba(255, 255, 255, .1)
                            border.color: Qt.rgba(120, 130, 140, .2)
                        }
                        contentItem: Text {
                            text: comboV2RaySecurity.displayText
                            color: "white"
                            leftPadding: 10
                            verticalAlignment: Text.AlignVCenter
                        }
                    }

                    Label {
                        text: qsTr("Enable UDP")
                        color: "white"
                    }

                    CheckBox {
                        id: checkboxV2RayEnableUdp
                        leftPadding: -3
                    }

                    Label {
                        text: qsTr("Network Security")
                        color: "white"
                    }

                    ComboBox {
                        id: comboV2RayNetworkSecurity
                        Layout.fillWidth: true
                        textRole: "text"
                        valueRole: "value"
                        model: ListModel{
                            ListElement { text: "None"; value: "none" }
                            ListElement { text: "TLS"; value: "tls" }
                        }
                        background: Rectangle {
                            color: Qt.rgba(255, 255, 255, .1)
                            border.color: Qt.rgba(120, 130, 140, .2)
                        }
                        contentItem: Text {
                            text: comboV2RayNetworkSecurity.displayText
                            color: "white"
                            leftPadding: 10
                            verticalAlignment: Text.AlignVCenter
                        }
                    }

                    Label {
                        text: qsTr("Allow Insecure")
                        color: "white"
                    }

                    CheckBox {
                        id: checkboxV2RayAllowInsecure
                        leftPadding: -2
                    }

                    Label {
                        text: qsTr("Network")
                        color: "white"
                    }

                    ComboBox {
                        id: comboV2RayNetwork
                        Layout.columnSpan: 3
                        Layout.fillWidth: true
                        textRole: "text"
                        valueRole: "value"
                        model: ListModel{
                            ListElement { text: "TCP"; value: "tcp" }
                            ListElement { text: "Websocket"; value: "ws" }
                        }
                        background: Rectangle {
                            color: Qt.rgba(255, 255, 255, .1)
                            border.color: Qt.rgba(120, 130, 140, .2)
                        }
                        contentItem: Text {
                            text: comboV2RayNetwork.displayText
                            color: "white"
                            leftPadding: 10
                            verticalAlignment: Text.AlignVCenter
                        }
                        onCurrentTextChanged: function() {
                            labelV2RayTcpHeaderType.visible = false
                            comboV2RayTcpHeaderType.visible = false
                            labelV2RayNetworkHost.visible = false
                            textV2RayNetworktHost.visible = false
                            labelV2RayNetworkPath.visible = false
                            textV2RayNetworkPath.visible = false

                            if ( comboV2RayNetwork.currentText === "TCP" ) {
                                labelV2RayTcpHeaderType.visible = true
                                comboV2RayTcpHeaderType.visible = true
                            } else if ( comboV2RayNetwork.currentText === "Websocket" ) {
                                labelV2RayNetworkHost.visible = true
                                textV2RayNetworktHost.visible = true
                                labelV2RayNetworkPath.visible = true
                                textV2RayNetworkPath.visible = true
                            }
                        }
                    }

                    Label {
                        id: labelV2RayTcpHeaderType
                        text: qsTr("TCP Header")
                        color: "white"
                    }

                    ComboBox {
                        id: comboV2RayTcpHeaderType
                        Layout.columnSpan: 3
                        Layout.fillWidth: true
                        textRole: "text"
                        valueRole: "value"
                        model: ListModel{
                            ListElement { text: "None"; value: "none" }
                            ListElement { text: "HTTP"; value: "http" }
                        }
                        background: Rectangle {
                            color: Qt.rgba(255, 255, 255, .1)
                            border.color: Qt.rgba(120, 130, 140, .2)
                        }
                        contentItem: Text {
                            text: comboV2RayTcpHeaderType.displayText
                            color: "white"
                            leftPadding: 10
                            verticalAlignment: Text.AlignVCenter
                        }
                    }

                    Label {
                        id: labelV2RayNetworkHost
                        text: qsTr("Host")
                        color: "white"
                    }

                    TextField {
                        id: textV2RayNetworktHost
                        color: "white"
                        Layout.fillWidth: true
                        placeholderText: qsTr("Example: example.com")
                        background: Rectangle {
                            color: Qt.rgba(255, 255, 255, .1)
                            border.color: Qt.rgba(120, 130, 140, .2)
                        }
                    }

                    Label {
                        id: labelV2RayNetworkPath
                        text: qsTr("Path")
                        color: "white"
                    }

                    TextField {
                        id: textV2RayNetworkPath
                        color: "white"
                        Layout.fillWidth: true
                        placeholderText: qsTr("Example: /ray")
                        background: Rectangle {
                            color: Qt.rgba(255, 255, 255, .1)
                            border.color: Qt.rgba(120, 130, 140, .2)
                        }
                    }

                    Button {
                        id: buttonV2RayAddServer
                        text: qsTr("Add Server")
                        contentItem: Text {
                            text: parent.text
                            color: "white"
                        }
                        background: Rectangle {
                            color: parent.enabled ? (parent.down ? "#2980b9" : "#3498db") : "#bdc3c7"
                            radius: 4
                        }
                        onClicked: function() {
                            buttonV2RayAddServer.enabled = false
                            var server = {
                                "serverName": textV2RayServerName.text,
                                "serverAddr": textV2RayServerAddr.text,
                                "serverPort": textV2RayServerPort.text,
                                "autoConnect": checkboxV2RayAutoConnect.checked,
                                "id": textV2RayId.text,
                                "alterId": textV2RayAlterId.text,
                                "security": comboV2RaySecurity.currentText,
                                "udp": checkboxV2RayEnableUdp.checked,
                                "network": comboV2RayNetwork.currentValue,
                                "tcpHeaderType": comboV2RayTcpHeaderType.currentValue,
                                "networkSecurity": comboV2RayNetworkSecurity.currentText,
                                "allowInsecure": checkboxV2RayAllowInsecure.checked,
                                "networkHost": textV2RayNetworktHost.text,
                                "networkPath": textV2RayNetworkPath.text
                            }
                            if (buttonV2RayAddServer.text === qsTr("Add Server")) {
                                AppProxy.addServer("V2Ray", JSON.stringify(server))
                            } else {
                                AppProxy.editServer(popUpServer.editServerName, "V2Ray", JSON.stringify(server))
                            }
                        }
                    }
                }

                GridLayout {
                    id: layoutServerShadowsocksManually
                    columns: 4
                    flow: GridLayout.LeftToRight
                    rowSpacing: 20
                    columnSpacing: 20
                    visible: false

                    Label {
                        text: qsTr("Server Name")
                        color: "white"
                        width: labelAddServerMethod.width
                    }

                    TextField {
                        id: textShadowsocksServerName
                        color: "white"
                        Layout.fillWidth: true
                        placeholderText: qsTr("Example: HongKong-Server-1")
                        background: Rectangle {
                            color: Qt.rgba(255, 255, 255, .1)
                            border.color: Qt.rgba(120, 130, 140, .2)
                        }
                    }

                    Label {
                        text: qsTr("Auto Connect")
                        color: "white"
                    }

                    CheckBox {
                        id: checkboxShadowsocksAutoConnect
                        leftPadding: -2
                    }

                    Label {
                        text: qsTr("Server Address")
                        color: "white"
                    }

                    TextField {
                        id: textShadowsocksServerAddr
                        color: "white"
                        Layout.fillWidth: true
                        placeholderText: qsTr("Example: hk.example.com")
                        background: Rectangle {
                            color: Qt.rgba(255, 255, 255, .1)
                            border.color: Qt.rgba(120, 130, 140, .2)
                        }
                    }

                    Label {
                        text: qsTr("Server Port")
                        color: "white"
                    }

                    TextField {
                        id: textShadowsocksServerPort
                        color: "white"
                        Layout.fillWidth: true
                        placeholderText: qsTr("Example: 8388")
                        background: Rectangle {
                            color: Qt.rgba(255, 255, 255, .1)
                            border.color: Qt.rgba(120, 130, 140, .2)
                        }
                    }

                    Label {
                        text: qsTr("Security")
                        color: "white"
                    }

                    ComboBox {
                        id: comboShadowsocksEncryptionMethod
                        Layout.fillWidth: true
                        model: ListModel{
                            ListElement { text: "AES-128-CFB" }
                            ListElement { text: "AES-128-CTR" }
                            ListElement { text: "AES-128-GCM" }
                            ListElement { text: "AES-192-CFB" }
                            ListElement { text: "AES-192-CTR" }
                            ListElement { text: "AES-192-GCM" }
                            ListElement { text: "AES-256-CFB" }
                            ListElement { text: "AES-256-CTR" }
                            ListElement { text: "AES-256-GCM" }
                            ListElement { text: "BF-CFB" }
                            ListElement { text: "CAMELLIA-128-CFB" }
                            ListElement { text: "CAMELLIA-192-CFB" }
                            ListElement { text: "CAMELLIA-256-CFB" }
                            ListElement { text: "CAST5-CFB" }
                            ListElement { text: "CHACHA20" }
                            ListElement { text: "CHACHA20-IETF" }
                            ListElement { text: "CHACHA20-IETF-POLY1305" }
                            ListElement { text: "DES-CFB" }
                            ListElement { text: "IDEA-CFB" }
                            ListElement { text: "RC4-MD5" }
                            ListElement { text: "SALSA20" }
                            ListElement { text: "SEED-CFB" }
                            ListElement { text: "SERPENT-256-CFB" }
                        }
                        background: Rectangle {
                            color: Qt.rgba(255, 255, 255, .1)
                            border.color: Qt.rgba(120, 130, 140, .2)
                        }
                        contentItem: Text {
                            text: comboShadowsocksEncryptionMethod.displayText
                            color: "white"
                            leftPadding: 10
                            verticalAlignment: Text.AlignVCenter
                        }
                    }

                    Label {
                        text: qsTr("Password")
                        color: "white"
                    }

                    TextField {
                        id: textShadowsocksPassword
                        color: "white"
                        Layout.fillWidth: true
                        background: Rectangle {
                            color: Qt.rgba(255, 255, 255, .1)
                            border.color: Qt.rgba(120, 130, 140, .2)
                        }
                    }

                    Label {
                        text: qsTr("Obfuscate Mode")
                        color: "white"
                    }

                    ComboBox {
                        id: comboObfsMode
                        Layout.fillWidth: true
                        textRole: "text"
                        valueRole: "value"
                        model: ListModel{
                            ListElement { text: "Disabled"; value: "" }
                            ListElement { text: "TLS"; value: "tls" }
                            ListElement { text: "HTTP"; value: "http" }
                            ListElement { text: "PLAIN"; value: "plain" }
                            ListElement { text: "HTTP SIMPLE"; value: "http_simple" }
                            ListElement { text: "HTTP POST"; value: "http_post" }
                            ListElement { text: "RANDOM HEAD"; value: "random_head" }
                            ListElement { text: "TLS1.2 TICKET AUTH"; value: "tls1.2_ticket_auth" }
                            ListElement { text: "TLS1.2 TICKET FASTAUTH"; value: "tls1.2_ticket_fastauth" }
                        }
                        background: Rectangle {
                            color: Qt.rgba(255, 255, 255, .1)
                            border.color: Qt.rgba(120, 130, 140, .2)
                        }
                        contentItem: Text {
                            text: comboObfsMode.displayText
                            color: "white"
                            leftPadding: 10
                            verticalAlignment: Text.AlignVCenter
                        }
                        onCurrentTextChanged: function() {
                            if (comboObfsMode.currentIndex < 3) {
                                // Shadowsocks
                                labelProtocol.visible = false
                                comboProtocol.visible = false
                                labelProtocolParameter.visible = false
                                textProtocolParameter.visible = false
                            } else {
                                // ShadowsocksR
                                labelProtocol.visible = true
                                comboProtocol.visible = true
                                labelProtocolParameter.visible = true
                                textProtocolParameter.visible = true
                            }
                        }
                    }

                    Label {
                        text: qsTr("Obfuscate Parameter")
                        color: "white"
                    }

                    TextField {
                        id: textObfsParameter
                        color: "white"
                        Layout.fillWidth: true
                        background: Rectangle {
                            color: Qt.rgba(255, 255, 255, .1)
                            border.color: Qt.rgba(120, 130, 140, .2)
                        }
                    }

                    Label {
                        id: labelProtocol
                        text: qsTr("Protocol")
                        color: "white"
                    }

                    ComboBox {
                        id: comboProtocol
                        Layout.fillWidth: true
                        model: ListModel{
                            ListElement { text: "Origin" }
                            ListElement { text: "AUTH_SHA1_V4" }
                            ListElement { text: "AUTH_AES128_MD5" }
                            ListElement { text: "AUTH_AES128_SHA1" }
                            ListElement { text: "AUTH_CHAIN_A" }
                            ListElement { text: "AUTH_CHAIN_B" }
                        }
                        background: Rectangle {
                            color: Qt.rgba(255, 255, 255, .1)
                            border.color: Qt.rgba(120, 130, 140, .2)
                        }
                        contentItem: Text {
                            text: comboProtocol.displayText
                            color: "white"
                            leftPadding: 10
                            verticalAlignment: Text.AlignVCenter
                        }
                    }

                    Label {
                        id: labelProtocolParameter
                        text: qsTr("Protocol Parameter")
                        color: "white"
                    }

                    TextField {
                        id: textProtocolParameter
                        color: "white"
                        Layout.fillWidth: true
                        background: Rectangle {
                            color: Qt.rgba(255, 255, 255, .1)
                            border.color: Qt.rgba(120, 130, 140, .2)
                        }
                    }

                    Button {
                        id: buttonShadowsocksAddServer
                        text: qsTr("Add Server")
                        contentItem: Text {
                            text: parent.text
                            color: "white"
                        }
                        background: Rectangle {
                            color: parent.enabled ? (parent.down ? "#2980b9" : "#3498db") : "#bdc3c7"
                            radius: 4
                        }
                        onClicked: function() {
                            buttonShadowsocksAddServer.enabled = false
                            var server = {
                                "serverName": textShadowsocksServerName.text,
                                "serverAddr": textShadowsocksServerAddr.text,
                                "serverPort": textShadowsocksServerPort.text,
                                "autoConnect": checkboxShadowsocksAutoConnect.checked,
                                "encryption": comboShadowsocksEncryptionMethod.currentText,
                                "password": textShadowsocksPassword.text,
                                "plugins": {
                                    "obfs": comboObfsMode.currentValue,
                                    "obfs-host": textObfsParameter.text
                                }
                            }
                            if (comboObfsMode.currentIndex >= 3) {
                                // ShadowsocksR
                                server["plugins"] = {
                                    "obfs": comboObfsMode.currentValue,
                                    "obfsparam": textObfsParameter.text,
                                    "protocol": comboProtocol.currentText,
                                    "protoparam": textProtocolParameter.text
                                }
                            }
                            if (buttonShadowsocksAddServer.text === qsTr("Add Server")) {
                                AppProxy.addServer("Shadowsocks", JSON.stringify(server))
                            } else {
                                AppProxy.editServer(popUpServer.editServerName, "Shadowsocks", JSON.stringify(server))
                            }
                        }
                    }
                }

                GridLayout {
                    id: layoutServerTrojanManually
                    columns: 4
                    flow: GridLayout.LeftToRight
                    rowSpacing: 20
                    columnSpacing: 20
                    visible: false

                    Label {
                        text: qsTr("Server Name")
                        color: "white"
                        width: labelAddServerMethod.width
                    }

                    TextField {
                        id: textTrojanServerName
                        color: "white"
                        Layout.fillWidth: true
                        placeholderText: qsTr("Example: HongKong-Server-1")
                        background: Rectangle {
                            color: Qt.rgba(255, 255, 255, .1)
                            border.color: Qt.rgba(120, 130, 140, .2)
                        }
                    }

                    Label {
                        text: qsTr("Auto Connect")
                        color: "white"
                    }

                    CheckBox {
                        id: checkboxTrojanAutoConnect
                        leftPadding: -2
                    }

                    Label {
                        text: qsTr("Server Address")
                        color: "white"
                    }

                    TextField {
                        id: textTrojanServerAddr
                        color: "white"
                        Layout.fillWidth: true
                        placeholderText: qsTr("Example: hk.example.com")
                        background: Rectangle {
                            color: Qt.rgba(255, 255, 255, .1)
                            border.color: Qt.rgba(120, 130, 140, .2)
                        }
                    }

                    Label {
                        text: qsTr("Server Port")
                        color: "white"
                    }

                    TextField {
                        id: textTrojanServerPort
                        color: "white"
                        Layout.fillWidth: true
                        placeholderText: qsTr("Example: 443")
                        background: Rectangle {
                            color: Qt.rgba(255, 255, 255, .1)
                            border.color: Qt.rgba(120, 130, 140, .2)
                        }
                    }

                    Label {
                        text: qsTr("Password")
                        color: "white"
                    }

                    TextField {
                        id: textTrojanPassword
                        color: "white"
                        Layout.fillWidth: true
                        background: Rectangle {
                            color: Qt.rgba(255, 255, 255, .1)
                            border.color: Qt.rgba(120, 130, 140, .2)
                        }
                    }

                    Label {
                        text: qsTr("UDP")
                        color: "white"
                    }

                    CheckBox {
                        id: checkboxTrojanEnableUdp
                        leftPadding: -3
                    }

                    Label {
                        text: qsTr("SNI")
                        color: "white"
                    }

                    TextField {
                        id: textTrojanSni
                        color: "white"
                        Layout.fillWidth: true
                        background: Rectangle {
                            color: Qt.rgba(255, 255, 255, .1)
                            border.color: Qt.rgba(120, 130, 140, .2)
                        }
                    }

                    Label {
                        text: qsTr("Allow Insecure")
                        color: "white"
                    }

                    CheckBox {
                        id: checkboxTrojanAllowInsecure
                        leftPadding: -3
                    }

                    Label {
                        text: qsTr("ALPN Protocols")
                        color: "white"
                    }

                    TextField {
                        id: textTrojanAlpn
                        color: "white"
                        Layout.fillWidth: true
                        Layout.columnSpan: 3
                        placeholderText: qsTr("Example: h2; http/1.1")
                        background: Rectangle {
                            color: Qt.rgba(255, 255, 255, .1)
                            border.color: Qt.rgba(120, 130, 140, .2)
                        }
                    }

                    Button {
                        id: buttonTrojanAddServer
                        text: qsTr("Add Server")
                        contentItem: Text {
                            text: parent.text
                            color: "white"
                        }
                        background: Rectangle {
                            color: parent.enabled ? (parent.down ? "#2980b9" : "#3498db") : "#bdc3c7"
                            radius: 4
                        }
                        onClicked: function() {
                            buttonTrojanAddServer.enabled = false
                            var server = {
                                "serverName": textTrojanServerName.text,
                                "serverAddr": textTrojanServerAddr.text,
                                "serverPort": textTrojanServerPort.text,
                                "autoConnect": checkboxTrojanAutoConnect.checked,
                                "password": textTrojanPassword.text,
                                "sni": textTrojanSni.text,
                                "udp": checkboxTrojanEnableUdp.checked,
                                "alpn": textTrojanAlpn.text,
                                "allowInsecure": checkboxTrojanAllowInsecure.checked
                            }
                            if (buttonTrojanAddServer.text === qsTr("Add Server")) {
                                AppProxy.addServer("Trojan", JSON.stringify(server))
                            } else {
                                AppProxy.editServer(popUpServer.editServerName, "Trojan", JSON.stringify(server))
                            }
                        }
                    }
                }

                GridLayout {
                    id: layoutServerSubscriptionUrl
                    columns: 2
                    flow: GridLayout.LeftToRight
                    rowSpacing: 20
                    columnSpacing: 20
                    visible: false

                    Label {
                        text: qsTr("Subscription URL")
                        color: "white"
                        rightPadding: 2
                    }

                    TextField {
                        id: textSubsriptionUrl
                        color: "white"
                        Layout.fillWidth: true
                        placeholderText: qsTr("Example: https://url/to/subscription or vmess://abcdefg")
                        background: Rectangle {
                            color: Qt.rgba(255, 255, 255, .1)
                            border.color: Qt.rgba(120, 130, 140, .2)
                        }
                    }

                    Button {
                        id: buttonSubscriptionAddServer
                        text: qsTr("Add Servers")
                        contentItem: Text {
                            text: parent.text
                            color: "white"
                        }
                        background: Rectangle {
                            color: parent.enabled ? (parent.down ? "#2980b9" : "#3498db") : "#bdc3c7"
                            radius: 4
                        }
                        onClicked: function() {
                            buttonSubscriptionAddServer.enabled = false
                            AppProxy.addServerUrl(textSubsriptionUrl.text)
                        }
                    }
                }

                GridLayout {
                    id: layoutServerJsonFiles
                    columns: 3
                    flow: GridLayout.LeftToRight
                    rowSpacing: 20
                    columnSpacing: 20
                    visible: false

                    Label {
                        text: qsTr("Config File Path")
                        color: "white"
                    }

                    TextField {
                        id: textConfigFilePath
                        color: "white"
                        Layout.fillWidth: true
                        placeholderText: qsTr("Example: /path/to/config.json")
                        background: Rectangle {
                            color: Qt.rgba(255, 255, 255, .1)
                            border.color: Qt.rgba(120, 130, 140, .2)
                        }
                    }

                    Button {
                        id: buttonSelectConfigFile
                        text: qsTr("Choose File")
                        contentItem: Text {
                            text: parent.text
                            color: "white"
                        }
                        background: Rectangle {
                            color: parent.enabled ? (parent.down ? "#2980b9" : "#3498db") : "#bdc3c7"
                            radius: 4
                        }
                        onClicked: function() {
                            fileDialog.open()
                        }
                    }

                    Button {
                        id: buttonConfigAddServer
                        text: qsTr("Add Servers")
                        contentItem: Text {
                            text: parent.text
                            color: "white"
                        }
                        background: Rectangle {
                            color: parent.enabled ? (parent.down ? "#2980b9" : "#3498db") : "#bdc3c7"
                            radius: 4
                        }
                        onClicked: function() {
                            var configFilePath = textConfigFilePath.text,
                            configFileType = comboAddServerMethod.currentValue

                            AppProxy.addServerConfigFile(configFilePath, configFileType)
                        }
                    }

                    FileDialog {
                        id: fileDialog
                        visible: false
                        modality: Qt.WindowModal
                        selectMultiple: false
                        selectFolder: false
                        nameFilters: [qsTr("Config File (*.json)")]
                        selectedNameFilter: qsTr("Config File (*.json)")
                        onAccepted: {
                            var path = fileDialog.fileUrl.toString()
                            path = path.replace(/^(file:\/{2})/,"") // remove prefixed "file://"
                            path = decodeURIComponent(path)         // unescape html codes like '%23' for '#'
                            if ( Qt.platform.os === "windows" || Qt.platform.os === "winrt" ) {
                                // Remove leading splash for Windows
                                // The path in Windows looks like /C:/Windows/System32
                                path = path.substring(1)
                            }
                            textConfigFilePath.text = path
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
        }
    }

    Popup {
        id: popUpSubscription
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        height: parent.height
        focus: true
        modal: true
        width: parent.width
        background: Rectangle {
            color: "#2e3e4e"
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 10
            spacing: 20

            RowLayout {
                Label {
                    text: qsTr("Server Subscriptions")
                    color: "white"
                    font.pixelSize: 20
                }

                Item {      // spacer item
                    Layout.fillWidth: true
                    Rectangle {
                        anchors.fill: parent
                        color: "transparent"
                    }
                }

                Button {
                    text: qsTr("Add Subscription")
                    contentItem: Text {
                        text: parent.text
                        color: "white"
                    }
                    background: Rectangle {
                        color: parent.enabled ? (parent.down ? "#8e44ad" : "#9b59b6") : "#bdc3c7"
                        radius: 4
                    }
                    onClicked: function() {
                        popUpSubscription.close()

                        layoutServer.resetPopUpServerFields("add")
                        comboAddServerMethod.currentIndex = 3
                        popUpServer.open()
                    }
                }

                Button {
                    id: buttonSyncServers
                    text: qsTr("Sync Servers")
                    contentItem: Text {
                        text: parent.text
                        color: "white"
                    }
                    background: Rectangle {
                        color: parent.enabled ? (parent.down ? "#2980b9" : "#3498db") : "#bdc3c7"
                        radius: 4
                    }
                    onClicked: function() {
                        menuItemSyncServers.enabled = true
                        buttonSyncServers.enabled = false
                        buttonSyncServers.text = qsTr("Please wait ...")
                        AppProxy.updateSubscriptionServers(menuSubscription.currentSubscription)
                    }
                }
            }

            Label {
                id: labelSubscriptionErrorMsg
                background: Rectangle {
                    color: "#ee8989"
                }
                color: "#652424"
                Layout.fillWidth: true
                padding: 10
                visible: false
                wrapMode: Text.Wrap
            }

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true

                ListView {
                    id: listViewSubscriptions
                    anchors.fill: parent
                    anchors.rightMargin: 5
                    flickableDirection: Flickable.HorizontalAndVerticalFlick
                    headerPositioning: ListView.OverlayHeader
                    clip: true

                    function getColumnWidth(index) {
                        switch (index) {
                        case 0:
                            return listViewServers.width * 0.1
                        case 1:
                            return listViewServers.width * 0.9
                        default:
                            return 0
                        }
                    }

                    header: Row {
                        spacing: 1
                        z: 4
                        function itemAt(index) {
                            return listViewSubscriptionRepeater.itemAt(index)
                        }
                        Repeater {
                            id: listViewSubscriptionRepeater
                            model: [qsTr("#"), qsTr("URL")]
                            Label {
                                text: modelData
                                color: "white"
                                font.bold: true
                                padding: 10
                                width: listViewSubscriptions.getColumnWidth(index)
                                background: Rectangle {
                                    color: "#354759"
                                }
                            }
                        }
                    }

                    model: listModelSubscriptions
                    delegate: Column {
                        Row {
                            spacing: 1
                            Repeater {
                                model: values
                                ItemDelegate {
                                    text: value
                                    width: listViewSubscriptions.getColumnWidth(index)

                                    contentItem: Text {
                                        clip: true
                                        color: "white"
                                        text: parent.text
                                    }
                                    background: MouseArea {
                                        anchors.fill: parent
                                        acceptedButtons: Qt.RightButton
                                        onClicked: function() {
                                            menuSubscription.x = parent.x + mouseX
                                            menuSubscription.y = parent.parent.parent.y + mouseY
                                            menuSubscription.currentSubscription = parent.parent.data[1].text
                                            menuSubscription.open()
                                        }
                                    }
                                }
                            }
                        }
                        Rectangle {
                            color: "#3b4d5d"
                            width: parent.width
                            height: 1
                        }
                    }

                    ListModel {
                        id: listModelSubscriptions
                    }

                    ScrollIndicator.horizontal: ScrollIndicator { }
                    ScrollIndicator.vertical: ScrollIndicator { }
                }
            }

            Menu {
                id: menuSubscription
                padding: 5
                property var currentSubscription

                MenuItem {
                    text: qsTr("Copy URL")
                    onTriggered: function() {
                        AppProxy.copyToClipboard(menuSubscription.currentSubscription)
                    }
                }

                MenuItem {
                    id: menuItemSyncServers
                    text: qsTr("Sync Servers")
                    onTriggered: function() {
                        menuItemSyncServers.enabled = false
                        AppProxy.updateSubscriptionServers(menuSubscription.currentSubscription)
                    }
                }

                MenuItem {
                    text: qsTr("Remove")
                    onTriggered: function() {
                        AppProxy.removeSubscriptionServers(menuSubscription.currentSubscription)
                    }
                }
            }
        }
    }

    Connections {
        target: AppProxy

        function getServerPrettyInformation(server) {
            var serverAddress = server["server"],
            serverPort = server["port"],
            serverName = server["name"] || serverAddress,
            status = server["connected"] ? qsTr("Connected") : qsTr("Disconnected"),
            latency = "latency" in server ?
                        (server["latency"] === -1 ?
                             qsTr("Timeout") : server["latency"].toString() + " ms") : qsTr("N/a")

            var protocol = ""
            if (server["type"] === "ss") {
                protocol = "Shadowsocks"
            } else if (server["type"] === "ssr") {
                protocol = "ShadowsocksR"
            } else if (server["type"] === "vmess") {
                protocol = "V2Ray"
            } else if (server["type"] === "trojan") {
                protocol = "Trojan"
            }
            return [
                        {value: serverName},
                        {value: serverAddress + ":" + serverPort},
                        {value: protocol},
                        {value: status},
                        {value: latency}
                    ]
        }

        function getSubscriptionUrls(servers) {
            var subscriptionUrls = []
            for (var i = 0; i < servers.length; ++ i) {
                if ("subscription" in servers[i]) {
                    var subscriptionUrl = servers[i]["subscription"]
                    if (subscriptionUrl.length === 0) {
                        continue
                    }
                    if (subscriptionUrls.indexOf(subscriptionUrl) === -1) {
                        subscriptionUrls.push(subscriptionUrl)
                    }
                }
            }
            return subscriptionUrls
        }

        function onServersReady(servers) {
            servers = JSON.parse(servers)
            listModelServers.clear()
            listModelSubscriptions.clear()

            var i = 0,
            subscriptionUrls = getSubscriptionUrls(servers)
            for (i = 0; i < servers.length; ++ i) {
                if ('name' in servers[i]) {
                    listModelServers.append({values: getServerPrettyInformation(servers[i])})
                }
            }
            for (i = 0; i < subscriptionUrls.length; ++ i) {
                listModelSubscriptions.append({values: [
                                                      {value: (i + 1).toString()},
                                                      {value: subscriptionUrls[i]}
                                                  ]})
            }
        }

        function onServerLatencyReady(serverLatency) {
            serverLatency = JSON.parse(serverLatency)
            // Refresh latency in server list
            for (var i = 0; i < listModelServers.count; ++ i) {
                var server = listModelServers.get(i),
                serverName = server.values.get(0).value

                if (serverName in serverLatency) {
                    var latency = serverLatency[serverName]
                    latency = latency === -1 ? qsTr("Timeout") : latency.toString() + " ms"
                    server.values.set(4, {value: latency})
                }
            }
            menuItemTestLatency.enabled = true
            menuItemTestAllLatency.enabled = true
        }

        function onServerConfigError(errorMsg) {
            var popUpButtons = [
                        buttonV2RayAddServer, buttonShadowsocksAddServer, buttonTrojanAddServer,
                        buttonSubscriptionAddServer, buttonConfigAddServer,
                        buttonSyncServers, menuItemSyncServers
                    ]
            for (var i = 0; i < popUpButtons.length; ++ i) {
                popUpButtons[i].enabled = true
            }
            buttonSyncServers.text = qsTr("Sync Servers")

            labelServerConfigErrorMsg.text = errorMsg
            labelSubscriptionErrorMsg.text = errorMsg
            labelServerConfigErrorMsg.visible = true
            labelSubscriptionErrorMsg.visible = true
        }

        function onServerConnectivityChanged(serverName, connected) {
            // Refresh connectivity for the server
            for (var i = 0; i < listModelServers.count; ++ i) {
                var _serverName = listModelServers.get(i).values.get(0).value
                if (_serverName === serverName) {
                    listModelServers.get(i).values.get(3).value =
                            connected ? qsTr("Connected") : qsTr("Disconnected")
                    break
                }
            }
        }

        function onServerChanged(serverName, serverConfig) {
            serverConfig = JSON.parse(serverConfig)

            for (var i = 0; i < listModelServers.count; ++ i) {
                var _serverName = listModelServers.get(i).values.get(0).value
                if (_serverName === serverName) {
                    listModelServers.set(i, {
                                             values: getServerPrettyInformation(serverConfig)
                                         })
                    break
                }
            }
            popUpServer.close()
            popUpSubscription.close()
        }

        function onServerRemoved(serverName) {
            for (var i = 0; i < listModelServers.count; ++ i) {
                var _serverName = listModelServers.get(i).values.get(0).value
                if (_serverName === serverName) {
                    listModelServers.remove(i)
                    break
                }
            }
        }

        function onServersChanged() {
            AppProxy.getServers()
            popUpServer.close()
            popUpSubscription.close()
        }

        function onServerDInfoReady(server) {
            server = JSON.parse(server)
            layoutServer.resetPopUpServerFields("name" in server ? "edit" : "add")

            // Set correct form in pop up window
            var protocol = server["type"]
            if (protocol === "vmess") {
                comboAddServerMethod.currentIndex = 0
                textV2RayServerName.text = server["name"] || ""
                textV2RayServerAddr.text = server["server"]
                textV2RayServerPort.text = server["port"]
                checkboxV2RayAutoConnect.checked = server["autoConnect"]
                textV2RayId.text = server["uuid"]
                textV2RayAlterId.text = server["alterId"]
                comboV2RaySecurity.currentIndex = comboV2RaySecurity.indexOfValue(server["cipher"])
                checkboxV2RayEnableUdp.checked = server["udp"]
                comboV2RayNetwork.currentIndex = comboV2RayNetwork.indexOfValue(server["network"])
                comboV2RayNetworkSecurity.currentIndex = server["tls"] ? 1 : 0
                checkboxV2RayAllowInsecure.checked = server["skip-cert-verify"]
                if (server["network"] === "tcp") {
                    comboV2RayTcpHeaderType.currentIndex = 0
                } else if (server["network"] === "http") {
                    comboV2RayNetwork.currentIndex = comboV2RayNetwork.indexOfValue("tcp")
                    comboV2RayTcpHeaderType.currentIndex = comboV2RayTcpHeaderType.indexOfValue("http")
                } else if (server["network"] === "ws") {
                    textV2RayNetworktHost.text = server["ws-headers"]["Host"]
                    textV2RayNetworkPath.text = server["ws-path"]
                }
            } else if (protocol === "ss" || protocol === "ssr") {
                comboAddServerMethod.currentIndex = 1
                textShadowsocksServerName.text = server["name"] || ""
                textShadowsocksServerAddr.text = server["server"]
                textShadowsocksServerPort.text = server["port"]
                checkboxShadowsocksAutoConnect.checked = server["autoConnect"]
                comboShadowsocksEncryptionMethod.currentIndex =
                        comboShadowsocksEncryptionMethod.find(
                            server["cipher"].toUpperCase())
                textShadowsocksPassword.text = server["password"]

                if ("plugin-opts" in server) {
                    // Shadowsocks
                    comboObfsMode.currentIndex = comboObfsMode.indexOfValue(
                                server["plugin-opts"]["mode"] || "")
                    textObfsParameter.text = server["plugin-opts"]["host"] || ""
                } else if ("obfs" in server) {
                    // ShadowsocksR
                    comboObfsMode.currentIndex = comboObfsMode.indexOfValue(server["obfs"] || "")
                    textObfsParameter.text = "obfs-param" in server ? server["obfs-param"] : ""
                    comboProtocol.currentIndex = comboProtocol.indexOfValue(server["protocol"].toUpperCase() || "")
                    textProtocolParameter.text = "protocol-param" in server ? server["protocol-param"] : ""
                }
            } else if (protocol === "trojan") {
                comboAddServerMethod.currentIndex = 2
                textTrojanServerName.text = server["name"] || ""
                textTrojanServerAddr.text = server["server"]
                textTrojanServerPort.text = server["port"]
                checkboxTrojanAutoConnect.checked = server["autoConnect"]
                textTrojanPassword.text = server["password"]
                textTrojanSni.text = server["sni"]
                textTrojanAlpn.text = server["alpn"].join("; ")
                checkboxTrojanEnableUdp.checked = server["udp"]
                checkboxTrojanAllowInsecure.checked = server["skip-cert-verify"]
            }
            popUpServer.editServerName = server["name"] || ""
            popUpServer.open()
        }
    }

    Component.onCompleted: function() {
        AppProxy.getServers()
    }

    function resetPopUpServerFields(propose) {
        // Clear text fields for V2Ray
        textV2RayServerName.text = ""
        textV2RayServerAddr.text = ""
        textV2RayServerPort.text = ""
        checkboxV2RayAutoConnect.checked = false
        textV2RayId.text = ""
        textV2RayAlterId.text = ""
        checkboxV2RayEnableUdp.checked = false
        comboV2RaySecurity.currentIndex = 0
        comboV2RayNetwork.currentIndex = 0
        comboV2RayNetworkSecurity.currentIndex = 0
        checkboxV2RayAllowInsecure.checked = false
        comboV2RayTcpHeaderType.currentIndex = 0
        textV2RayNetworktHost.text = ""
        textV2RayNetworkPath.text = ""
        // Clear text fields for Shadowsocks
        textShadowsocksServerName.text = ""
        textShadowsocksServerAddr.text = ""
        textShadowsocksServerPort.text = ""
        checkboxShadowsocksAutoConnect.checked = false
        comboShadowsocksEncryptionMethod.currentIndex = 0
        textShadowsocksPassword.text = ""
        comboObfsMode.currentIndex = 0
        textObfsParameter.text = ""
        // Clear text fields for Trojan
        textTrojanServerName.text = ""
        checkboxTrojanAutoConnect.checked = false
        textTrojanServerAddr.text = ""
        textTrojanServerPort.text = ""
        textTrojanPassword.text = ""
        textTrojanSni.text = ""
        checkboxTrojanEnableUdp.checked = false
        textTrojanAlpn.text = ""
        checkboxTrojanAllowInsecure.checked = false
        // Clear text fields for subscrption
        textSubsriptionUrl.text = ""
        // Clear text fields for config files
        textConfigFilePath.text = ""
        // Initialize controls for editing or creating
        var i = 0,
        popUpButtons = [
                    buttonV2RayAddServer, buttonShadowsocksAddServer, buttonTrojanAddServer,
                    buttonSubscriptionAddServer, buttonConfigAddServer
                ]
        for (i = 0; i < popUpButtons.length; ++ i) {
            popUpButtons[i].enabled = true
        }
        if (propose === "edit") {
            labelServerInformation.visible = true
            labelAddServerMethod.visible = false
            comboAddServerMethod.visible = false
            for (i = 0; i < popUpButtons.length; ++ i) {
                popUpButtons[i].text = qsTr("Edit Server")
            }
        } else {
            labelServerInformation.visible = false
            labelAddServerMethod.visible = true
            comboAddServerMethod.visible = true
            for (i = 0; i < popUpButtons.length; ++ i) {
                popUpButtons[i].text = qsTr("Add Server")
            }
        }
        labelServerConfigErrorMsg.visible = false
    }
}
