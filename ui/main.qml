import QtQuick 2.12
import QtQuick.Window 2.12
import Qt.labs.platform 1.1

Window {
    visible: true
    width: 960
    height: 640
    minimumWidth: 640
    minimumHeight: 480
    title: qsTr("V2Ray Desktop")

    // Center the window
    x: Screen.width / 2 - width / 2
    y: Screen.height / 2 - height / 2

    SystemTrayIcon {
        visible: true
        icon.source: Qt.platform.os == "osx" ? "qrc:/images/v2ray.gray.png" : "qrc:/images/v2ray.png"

        menu: Menu {
            MenuItem {
                text: qsTr("V2Ray Desktop")
                enabled: false
            }

            MenuItem {
                id: triggerMenuItem
                text: qsTr("Turn V2Ray Desktop On")
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
                text: qsTr("Check for Updates")
            }

            MenuItem {
                text: qsTr("Feedback")
            }

            MenuItem {
                text: qsTr("About")
            }

            MenuItem {
                text: qsTr("Quit V2Ray Desktop")
                onTriggered: Qt.quit()
                shortcut: StandardKey.Quit
            }
        }
    }
}
