import QtQuick 2.9
import QtQuick.Controls 2.9
import Qt.labs.platform 1.1

import com.v2ray.desktop.AppProxy 1.0

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

        menu: Menu {
            MenuItem {
                id: appName
                text: qsTr("V2Ray Desktop")
                enabled: false
            }

            MenuItem {
                id: triggerV2RayCore
                text: qsTr("Turn V2Ray On")
                onTriggered: function() {
                    if (triggerV2RayCore.text === qsTr("V2Ray Core Installing ...")) {
                        appProxy.getV2RayCoreStatus()
                    } else if (triggerV2RayCore.text === qsTr("Turn V2Ray On")) {
                        appProxy.setV2RayCoreRunning(true)
                    } else if (triggerV2RayCore.text === qsTr("Turn V2Ray Off")) {
                        appProxy.setV2RayCoreRunning(false)
                    }
                }
            }

            MenuItem {
                separator: true
            }

            MenuItem {
                text: qsTr("PAC Mode")
                checkable: true
                checked: true
            }

            MenuItem {
                text: qsTr("Global Mode")
                checkable: true
            }

            MenuItem {
                text: qsTr("Manual Mode")
                checkable: true
            }

            MenuItem {
                separator: true
            }

            MenuItem {
                text: qsTr("Preferences")
                onTriggered: function() {
                    mouseAreaSettings.clicked(null)
                    appWindow.show()
                }
            }

            MenuItem {
                id: serversMenuItem
                text: qsTr("Servers")
            }

            MenuItem {
                text: qsTr("Scan QR Code on the Screen")
            }

            MenuItem {
                separator: true
            }

            MenuItem {
                text: qsTr("Logs")
                onTriggered: function() {
                    mouseAreaLogs.clicked(null)
                    appWindow.show()
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
                        font.pointSize: 14
                        x: 40
                        y: 10
                    }

                    MouseArea {
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
                        font.pointSize: 14
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
                        font.pointSize: 14
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
                        text: qsTr("Settings")
                        font.pointSize: 14
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
                        font.pointSize: 14
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
                        font.pointSize: 14
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

        AppProxy {
            id: appProxy

            onAppVersionReady: function(appVersion) {
                appName.text = qsTr("V2Ray Desktop") + " " + appVersion
            }
            onV2RayCoreStatusReady: function(v2RayCoreStatus) {
                if (v2RayCoreStatus === "Not Installed") {
                    triggerV2RayCore.text = qsTr("V2Ray Core Installing ...")
                } else if (v2RayCoreStatus === "Stopped") {
                    triggerV2RayCore.text = qsTr("Turn V2Ray On")
                } else if (v2RayCoreStatus === "Running") {
                    triggerV2RayCore.text = qsTr("Turn V2Ray Off")
                }
            }
            onV2RayRunningStatusChanging: function(isChanged) {
                if (isChanged) {
                    if (triggerV2RayCore.text === qsTr("Turn V2Ray On")) {
                        triggerV2RayCore.text = qsTr("Turn V2Ray Off")
                    } else {
                        triggerV2RayCore.text = qsTr("Turn V2Ray On")
                    }
                }
            }
        }

        Component.onCompleted: {
            // Get App Version
            appProxy.getAppVersion()
            // Start V2Ray Core Automatically
            appProxy.setV2RayCoreRunning(true)
        }
}
