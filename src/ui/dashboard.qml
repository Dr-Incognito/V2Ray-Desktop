import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14

import com.v2ray.desktop.AppProxy 1.0

ColumnLayout {
    anchors.fill: parent
    anchors.margins: 10
    spacing: 20

    RowLayout {
        Image {
            source: "qrc:///images/icon-dashboard.svg"
            sourceSize.width: 40
            sourceSize.height: 40
            mipmap: true
        }

        Text {
            text: qsTr("Dashboard")
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
            text: qsTr("Current Status")
            color: "white"
            font.bold: true
        }

        Label {
            id: labelCurrentStatus
            text: qsTr("N/a")
            color: "white"
        }

        Label {
            text: qsTr("Proxy Settings")
            color: "white"
            font.bold: true
        }

        Label {
            id: labelProxySettings
            text: qsTr("N/a")
            color: "white"
        }

        Label {
            text: qsTr("Operating System")
            color: "white"
            font.bold: true
        }

        Label {
            id: labelOperatingSystem
            text: qsTr("N/a")
            color: "white"
        }

        Label {
            text: qsTr("V2Ray Desktop Version")
            color: "white"
            font.bold: true
        }

        Label {
            id: labelAppVersion
            text: qsTr("N/a")
            color: "white"
        }

        Label {
            text: qsTr("V2Ray Core Version")
            color: "white"
            font.bold: true
        }

        Label {
            id: labelV2rayVersion
            text: qsTr("N/a")
            color: "white"
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

        onAppVersionReady: function(appVersion) {
            labelAppVersion.text = appVersion
        }

        onV2RayCoreVersionReady: function(v2RayVersion) {
            labelV2rayVersion.text = v2RayVersion
        }

        onOperatingSystemReady: function(operatingSystem) {
            labelOperatingSystem.text = operatingSystem
        }
    }

    Component.onCompleted: function() {
        AppProxy.getAppVersion()
        AppProxy.getOperatingSystem()
        AppProxy.getV2RayCoreVersion()
    }
}
