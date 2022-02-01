import QtQuick 2.14
import QtQuick.Controls 2.14
import Qt.labs.platform 1.1

import com.v2ray.desktop.AppProxy 2.2

ApplicationWindow {
    id: appWindow
    visible: true
    width: 960
    height: 640
    minimumWidth: 640
    minimumHeight: 480
    title: qsTr("V2Ray Desktop")
    x: (screen.width - appWindow.width) / 2
    y: (screen.height - appWindow.height) / 2
    property bool firstRun: true
    property string currentSysProxyProtocol: ""

    onClosing: function() {
        appWindow.hide()
    }

    Shortcut {
        sequence: StandardKey.Close
        onActivated: function() {
            appWindow.close()
        }
    }

    SystemTrayIcon {
        visible: true
        icon.source: Qt.platform.os == "osx" ? "qrc:/images/v2ray.gray.png" : "qrc:/images/v2ray.png"
        icon.mask: true

        menu: Menu {
            MenuItem {
                id: appName
                text: qsTr("V2Ray Desktop")
                enabled: false
            }

            MenuItem {
                id: triggerV2RayCore
                text: qsTr("Turn V2Ray Desktop On")
                property bool isV2RayRunning: false

                onTriggered: function() {
                    if (!isV2RayRunning) {
                        AppProxy.setV2RayCoreRunning(true)
                    } else if (isV2RayRunning) {
                        AppProxy.setV2RayCoreRunning(false)
                    }
                }
            }

            MenuSeparator {}

            MenuItem {
                id: menuItemRuleMode
                text: qsTr("Rule Mode")
                checkable: true
                enabled: false
                onTriggered: function() {
                  AppProxy.setProxyMode("Rule")
                }
            }

            MenuItem {
                id: menuItemGlobalMode
                text: qsTr("Global Mode")
                checkable: true
                enabled: false
                onTriggered: function() {
                  AppProxy.setProxyMode("Global")
                }
            }

            MenuItem {
                id: menuItemDirectMode
                text: qsTr("Direct Mode")
                checkable: true
                checked: true
                onTriggered: function() {
                  AppProxy.setProxyMode("Direct")
                }
            }

            MenuSeparator {}

            MenuItem {
                id: menuItemSetHttpProxy
                text: qsTr("Set System Proxy (HTTP)")
                checkable: true
                enabled: false
                onTriggered: function() {
                  menuItemSetSocksProxy.checked = false
                  AppProxy.setSystemProxy(menuItemSetHttpProxy.checked, "http")
                }
            }

            MenuItem {
                id: menuItemSetSocksProxy
                text: qsTr("Set System Proxy (SOCKS)")
                checkable: true
                enabled: false
                visible: Qt.platform.os == "windows" ? false : true
                onTriggered: function() {
                  menuItemSetHttpProxy.checked = false
                  AppProxy.setSystemProxy(menuItemSetSocksProxy.checked, "socks")
                }
            }

            MenuSeparator {}

            MenuItem {
                text: qsTr("Dashboard")
                onTriggered: function() {
                    mouseAreaDashboard.clicked(null)
                    appWindow.show()
                    appWindow.requestActivate()
                    appWindow.raise()
                }
            }

            MenuItem {
                id: menuItemServers
                text: qsTr("Servers")
                onTriggered: function() {
                    mouseAreaServers.clicked(null)
                    appWindow.show()
                    appWindow.requestActivate()
                    appWindow.raise()
                }
            }

            MenuItem {
                text: Qt.platform.os == "osx" ? qsTr("Preferences") : qsTr("Settings")
                onTriggered: function() {
                    mouseAreaSettings.clicked(null)
                    appWindow.show()
                    appWindow.requestActivate()
                    appWindow.raise()

                }
            }

            MenuItem {
                text: qsTr("Scan QR Code on the Screen")
                onTriggered: function() {
                    AppProxy.scanQrCodeScreen()
                    mouseAreaServers.clicked(null)
                    appWindow.show()
                    appWindow.requestActivate()
                    appWindow.raise()
                }
            }

            MenuSeparator {}

            MenuItem {
                text: qsTr("Logs")
                onTriggered: function() {
                    mouseAreaLogs.clicked(null)
                    appWindow.show()
                    appWindow.requestActivate()
                    appWindow.raise()
                }
            }

            MenuItem {
                text: qsTr("Feedback")
                onTriggered: Qt.openUrlExternally("https://github.com/Dr-Incognito/V2Ray-Desktop/issues")
            }

            MenuItem {
                text: qsTr("About")
                onTriggered: function() {
                    mouseAreaAbout.clicked(null)
                    appWindow.show()
                    appWindow.requestActivate()
                    appWindow.raise()
                }
            }

            MenuItem {
                text: qsTr("Quit V2Ray Desktop")
                onTriggered: Qt.quit()
                shortcut: StandardKey.Quit
            }
        }
    }

    Rectangle{
            id: sidebar
            color: "#293846"
            width: 240
            height: parent.height
            anchors.top: parent.top
            anchors.bottom: parent.bottom

            Image {
                id: logo
                source: "qrc:///images/logo.png"
                width: 200
                height: 50
                x: 20
                y: 10
                mipmap: true
            }

            Item {
                width: parent.width
                height: parent.height - 70
                y: 70

                Rectangle {
                    id: navDashboard
                    color: "#354759"
                    width: parent.width
                    height: 40
                    y: 0

                    Image {
                        source: "qrc:///images/icon-dashboard.svg"
                        height: 14
                        width: 14
                        x: 20
                        y: 12
                    }

                    Text {
                        color: "white"
                        text: qsTr("Dashboard")
                        font.pointSize: Qt.platform.os == "windows" ? 12 : 14
                        x: 40
                        y: 10
                    }

                    MouseArea {
                        id: mouseAreaDashboard
                        width: parent.width
                        height: parent.height

                        onClicked: function() {
                            navServers.color = "#263441"
                            navRules.color = "#263441"
                            navSettings.color = "#263441"
                            navAbout.color = "#263441"
                            navLogs.color = "#263441"
                            navDashboard.color = "#354759"
                            pageLoader.source = "dashboard.qml"
                        }
                    }
                }

                Rectangle {
                    id: navServers
                    color: "#263441"
                    width: parent.width
                    height: 40
                    y: 40

                    MouseArea {
                        id: mouseAreaServers
                        width: parent.width
                        height: parent.height

                        onClicked: function() {
                            navDashboard.color = "#263441"
                            navRules.color = "#263441"
                            navSettings.color = "#263441"
                            navAbout.color = "#263441"
                            navLogs.color = "#263441"
                            navServers.color = "#354759"
                            pageLoader.source = "servers.qml"
                        }
                    }

                    Image {
                        source: "qrc:///images/icon-servers.svg"
                        height: 14
                        width: 14
                        x: 20
                        y: 12
                    }

                    Text {
                        color: "white"
                        text: qsTr("Servers")
                        font.pointSize: Qt.platform.os == "windows" ? 12 : 14
                        x: 40
                        y: 10
                    }
                }

                Rectangle {
                    id: navRules
                    color: "#263441"
                    width: parent.width
                    height: 40
                    y: 80

                    MouseArea {
                        width: parent.width
                        height: parent.height

                        onClicked: function() {
                            navDashboard.color = "#263441"
                            navServers.color = "#263441"
                            navSettings.color = "#263441"
                            navAbout.color = "#263441"
                            navLogs.color = "#263441"
                            navRules.color = "#354759"
                            pageLoader.source = "rules.qml"
                        }
                    }

                    Image {
                        source: "qrc:///images/icon-rules.svg"
                        height: 14
                        width: 14
                        x: 20
                        y: 12
                    }

                    Text {
                        color: "white"
                        text: qsTr("Rules")
                        font.pointSize: Qt.platform.os == "windows" ? 12 : 14
                        x: 40
                        y: 10
                    }
                }

                Rectangle {
                    id: navSettings
                    color: "#263441"
                    width: parent.width
                    height: 40
                    y: 120

                    MouseArea {
                        id: mouseAreaSettings
                        width: parent.width
                        height: parent.height

                        onClicked: function() {
                            navDashboard.color = "#263441"
                            navServers.color = "#263441"
                            navRules.color = "#263441"
                            navAbout.color = "#263441"
                            navLogs.color = "#263441"
                            navSettings.color = "#354759"
                            pageLoader.source = "settings.qml"
                        }
                    }

                    Image {
                        source: "qrc:///images/icon-settings.svg"
                        height: 14
                        width: 14
                        x: 20
                        y: 12
                    }

                    Text {
                        color: "white"
                        text: Qt.platform.os == "osx" ? qsTr("Preferences") : qsTr("Settings")
                        font.pointSize: Qt.platform.os == "windows" ? 12 : 14
                        x: 40
                        y: 10
                    }
                }

                Rectangle {
                    id: navLogs
                    color: "#263441"
                    width: parent.width
                    height: 40
                    y: 160

                    MouseArea {
                        id: mouseAreaLogs
                        width: parent.width
                        height: parent.height

                        onClicked: function() {
                            navDashboard.color = "#263441"
                            navServers.color = "#263441"
                            navRules.color = "#263441"
                            navSettings.color = "#263441"
                            navAbout.color = "#263441"
                            navLogs.color = "#354759"
                            pageLoader.source = "logs.qml"
                        }
                    }

                    Image {
                        source: "qrc:///images/icon-logs.svg"
                        height: 14
                        width: 14
                        x: 20
                        y: 12
                    }

                    Text {
                        color: "white"
                        text: qsTr("Logs")
                        font.pointSize: Qt.platform.os == "windows" ? 12 : 14
                        x: 40
                        y: 10
                    }
                }

                Rectangle {
                    id: navAbout
                    color: "#263441"
                    width: parent.width
                    height: 40
                    y: 200

                    MouseArea {
                        id: mouseAreaAbout
                        width: parent.width
                        height: parent.height

                        onClicked: function() {
                            navDashboard.color = "#263441"
                            navServers.color = "#263441"
                            navRules.color = "#263441"
                            navSettings.color = "#263441"
                            navLogs.color = "#263441"
                            navAbout.color = "#354759"
                            pageLoader.source = "about.qml"
                        }
                    }

                    Image {
                        source: "qrc:///images/icon-about.svg"
                        height: 14
                        width: 14
                        x: 20
                        y: 12
                    }

                    Text {
                        color: "white"
                        text: qsTr("About")
                        font.pointSize: Qt.platform.os == "windows" ? 12 : 14
                        x: 40
                        y: 10
                    }
                }
            }
        }

        Rectangle {
            id: content
            color: "#2e3e4e"
            width: parent.width - 240
            height: parent.height
            anchors.left: sidebar.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom

            Loader {
                id: pageLoader
                anchors.fill: parent
                source: "dashboard.qml"
            }
        }

        Connections {
            target: AppProxy

            function onAppVersionReady(appVersion) {
                appName.text = qsTr("V2Ray Desktop") + " " + appVersion
            }

            function onAppConfigReady(config) {
                config = JSON.parse(config)
                if (appWindow.firstRun && config["hideWindow"]) {
                  appWindow.close()
                  appWindow.firstRun = false
                }
                if (config["enableSysProxy"]) {
                  appWindow.currentSysProxyProtocol = config["defaultSysProxyProtocol"]
                }
            }

            function onV2RayCoreStatusReady(isRunning) {
                if (!isRunning) {
                    triggerV2RayCore.text = qsTr("Turn V2Ray Desktop On")
                    triggerV2RayCore.isV2RayRunning = false
                    menuItemRuleMode.enabled = false
                    menuItemGlobalMode.enabled = false
                    menuItemSetHttpProxy.enabled = false
                    menuItemSetSocksProxy.enabled = false
                    menuItemRuleMode.checked = false
                    menuItemGlobalMode.checked = false
                    menuItemDirectMode.checked = true
                    menuItemSetHttpProxy.checked = false
                    menuItemSetSocksProxy.checked = false
                    AppProxy.setSystemProxy(false)
                } else {
                    triggerV2RayCore.text = qsTr("Turn V2Ray Desktop Off")
                    triggerV2RayCore.isV2RayRunning = true
                    menuItemRuleMode.enabled = true
                    menuItemGlobalMode.enabled = true
                    menuItemSetHttpProxy.enabled = true
                    menuItemSetSocksProxy.enabled = true

                    // Set system proxy automatically
                    AppProxy.setProxyMode()

                    if (appWindow.currentSysProxyProtocol === "http") {
                        AppProxy.setSystemProxy(true, "http")
                        menuItemSetHttpProxy.checked = true
                    } else if (appWindow.currentSysProxyProtocol === "socks") {
                        AppProxy.setSystemProxy(true, "socks")
                        menuItemSetSocksProxy.checked = true
                    }
                }
            }

            function onProxyModeChanged(proxyMode) {
                menuItemRuleMode.checked = false
                menuItemGlobalMode.checked = false
                menuItemDirectMode.checked = false

                if (proxyMode === "Rule") {
                    menuItemRuleMode.checked = true
                } else if (proxyMode === "Global") {
                    menuItemGlobalMode.checked = true
                } else if (proxyMode === "Direct") {
                    menuItemDirectMode.checked = true
                }
            }
        }

        Component.onCompleted: {
            // Get App Version
            AppProxy.getAppVersion()
            // Get App Config
            AppProxy.getAppConfig()
            // Translate
            AppProxy.retranslate()
            // Start V2Ray Core automatically
            AppProxy.setV2RayCoreRunning(true)
        }
}
