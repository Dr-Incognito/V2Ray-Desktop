import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Dialogs 1.3

import com.v2ray.desktop.AppProxy 1.3

ColumnLayout {
    anchors.fill: parent
    anchors.margins: 10
    spacing: 20

    RowLayout {
        Image {
            source: "qrc:///images/icon-rules.svg"
            sourceSize.width: 40
            sourceSize.height: 40
        }

        Text {
            text: qsTr("Rules")
            color: "white"
            font.pointSize: 24
        }
    }

    GridLayout {
        columns: 3
        flow: GridLayout.LeftToRight
        rowSpacing: 20
        columnSpacing: 20

        Label {
            text: qsTr("Proxy Mode")
            color: "white"
        }

        ComboBox {
            id: comboProxyMode
            Layout.columnSpan: 2
            Layout.fillWidth: true
            textRole: "text"
            valueRole: "value"
            model: ListModel{
                ListElement { text: "PAC Mode"; value: "pac" }
                ListElement { text: "Global Mode"; value: "global" }
                ListElement { text: "Manual Mode"; value: "manual" }
            }
            background: Rectangle {
                color: Qt.rgba(255, 255, 255, .1)
                border.color: Qt.rgba(120, 130, 140, .2)
            }
            contentItem: Text {
                text: comboProxyMode.displayText
                color: "white"
                leftPadding: 10
                verticalAlignment: Text.AlignVCenter
            }
        }

        Label {
            text: qsTr("GFW List URL")
            color: "white"
        }

        TextField {
            id: textGfwListUrl
            color: "white"
            Layout.columnSpan: 2
            Layout.fillWidth: true
            placeholderText: qsTr("Example: https://url/to/gfwlist.txt")
            background: Rectangle {
                color: Qt.rgba(255, 255, 255, .1)
                border.color: Qt.rgba(120, 130, 140, .2)
            }
        }

        Label {
            text: qsTr("GFW List Last Updated on")
            color: "white"
        }

        Label {
            id: labelGfwLastUpdatedTime
            Layout.fillWidth: true
            color: "white"
        }

        Button {
            id: buttonUpdateGfwList
            text: qsTr("Update GFW List Now")
            contentItem: Text {
                id: buttonUpdateGfwListContentItem
                text: parent.text
                color: "#3498db"
            }
            background: Rectangle {
                color: "#2e3e4e"
                radius: 4
            }
            onClicked: function() {
              buttonUpdateGfwList.enabled = false
              buttonUpdateGfwListContentItem.color = "white"
              buttonUpdateGfwList.text = qsTr("Updating ...")
              AppProxy.updateGfwList(textGfwListUrl.text)
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
                AppProxy.setSystemProxyMode(comboProxyMode.currentValue)
                AppProxy.setGfwListUrl(textGfwListUrl.text)
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
            comboProxyMode.currentIndex = comboProxyMode.indexOfValue(config["proxyMode"])
            textGfwListUrl.text = config["gfwListUrl"]
            labelGfwLastUpdatedTime.text = config["gfwListLastUpdated"]
        }

        onAppConfigChanged: function() {
            messageDialog.open()
        }

        onProxyModeChanged: function(proxyMode) {
            comboProxyMode.currentIndex = comboProxyMode.indexOfValue(proxyMode)
        }

        onGfwListUpdated: function(updatedTime) {
            buttonUpdateGfwList.text = qsTr("Update GFW List Now")
            buttonUpdateGfwListContentItem.color = "#3498db"
            buttonUpdateGfwList.enabled = true
            labelGfwLastUpdatedTime.text = updatedTime
        }
    }

    Component.onCompleted: function() {
        AppProxy.getAppConfig()
    }
}
