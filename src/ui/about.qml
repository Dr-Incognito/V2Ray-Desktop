import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14

import com.v2ray.desktop.AppProxy 2.2

ColumnLayout {
    anchors.fill: parent
    anchors.margins: 10
    spacing: 20

    RowLayout {
        Image {
            source: "qrc:///images/icon-about.svg"
            sourceSize.width: 40
            sourceSize.height: 40
            mipmap: true
        }

        Text {
            text: qsTr("About")
            color: "white"
            font.pointSize: Qt.platform.os == "windows" ? 20 : 24
        }
    }

    GridLayout {
        columns: 3
        flow: GridLayout.LeftToRight
        rowSpacing: 20
        columnSpacing: 20

        Label {
            text: qsTr("V2Ray Desktop Version")
            color: "white"
            font.bold: true
        }

        Label {
            id: labelAppVersion
            text: "N/a"
            color: "white"
            property string value: "N/a"
        }

        Button {
            id: buttonAppCheckUpdates
            text: qsTr("Check for updates")
            contentItem: Text {
                text: parent.text
                color: parent.enabled ? "#3498db" : "#ccc"
            }
            background: Rectangle {
                color: "#2e3e4e"
                radius: 4
            }
            onClicked: function() {
                buttonAppCheckUpdates.text = qsTr("Checking updates ...")
                buttonAppCheckUpdates.enabled = false
                AppProxy.getLatestRelease("v2ray-desktop")
            }
        }

        Button {
            id: buttonAppUpgrade
            text: qsTr("Upgrade")
            visible: false
            property var value

            contentItem: Text {
                text: parent.text
                color: parent.enabled ? "#3498db" : "#ccc"
            }
            background: Rectangle {
                color: "#2e3e4e"
                radius: 4
            }
            onClicked: function() {
                Qt.openUrlExternally(
                    "https://github.com/Dr-Incognito/V2Ray-Desktop/releases/tag/" +
                    buttonAppUpgrade.value.substr(1))
            }
        }

        Label {
            text: qsTr("Clash Version")
            color: "white"
            font.bold: true
        }

        Label {
            id: labelV2rayVersion
            text: "N/a"
            color: "white"
            property string value: "N/a"
        }

        Button {
            id: buttonV2RayCheckUpdates
            text: qsTr("Check for updates")
            contentItem: Text {
                text: parent.text
                color: parent.enabled ? "#3498db" : "#ccc"
            }
            background: Rectangle {
                color: "#2e3e4e"
                radius: 4
            }
            onClicked: function() {
                buttonV2RayCheckUpdates.text = qsTr("Checking updates ...")
                buttonV2RayCheckUpdates.enabled = false
                AppProxy.getLatestRelease("v2ray-core")
            }
        }

        Label {
            text: qsTr("Project Page")
            color: "white"
            font.bold: true
        }

        Label {
            color: "white"
            text: "https://github.com/Dr-Incognito/V2Ray-Desktop"
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

    Connections {
        target: AppProxy

        function onAppVersionReady(appVersion) {
            labelAppVersion.text = labelAppVersion.value = appVersion
        }

        function onV2RayCoreVersionReady(v2RayVersion) {
            labelV2rayVersion.text = labelV2rayVersion.value = v2RayVersion
        }

        function onLatestReleaseReady(name, latestVersion) {
            var buttonCheckUpdates, buttonUpgrade, labelVersion

            if (name === "v2ray-core") {
                buttonCheckUpdates = buttonV2RayCheckUpdates
                buttonUpgrade = buttonV2RayUpgrade
                labelVersion = labelV2rayVersion
            } else if (name === "v2ray-desktop") {
                buttonCheckUpdates = buttonAppCheckUpdates
                buttonUpgrade = buttonAppUpgrade
                labelVersion = labelAppVersion
            }
            if (latestVersion.length === 0) {
                labelVersion.text = labelVersion.value + " (" +
                    qsTr("Already the latest verion") + ")"
            } else {
                labelVersion.text = labelVersion.value + " (" +
                    qsTr("Newer verion available: ") + latestVersion + ")"
                buttonCheckUpdates.visible = false
                buttonUpgrade.visible = true
                buttonUpgrade.value = latestVersion
            }
            buttonCheckUpdates.text = qsTr("Check for updates")
            buttonCheckUpdates.enabled = true
        }

        function onLatestReleaseError(name, errorMsg) {
            var buttonCheckUpdates, buttonUpgrade, labelVersion

            if (name === "v2ray-core") {
                buttonCheckUpdates = buttonV2RayCheckUpdates
                labelVersion = labelV2rayVersion
            } else if (name === "v2ray-desktop") {
                buttonCheckUpdates = buttonAppCheckUpdates
                labelVersion = labelAppVersion
            }
            labelVersion.text = labelVersion.value + " (" + errorMsg + ")"
            buttonCheckUpdates.text = qsTr("Check for updates")
            buttonCheckUpdates.enabled = true
        }
    }

    Component.onCompleted: function() {
        AppProxy.getAppVersion()
        AppProxy.getV2RayCoreVersion()
    }
}
