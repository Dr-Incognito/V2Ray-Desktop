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
            source: "qrc:///images/icon-rules.svg"
            sourceSize.width: 40
            sourceSize.height: 40
        }

        Text {
            text: qsTr("Rules")
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
        columns: 3
        flow: GridLayout.LeftToRight
        rowSpacing: 20
        columnSpacing: 20

        Label {
            text: qsTr("GFW List Last Updated on")
            color: "white"
            font.pointSize: 10.5
        }

        Label {
            id: labelGfwLastUpdatedTime
            Layout.fillWidth: true
            color: "white"
            font.pointSize: 10.5
        }

        Button {
            id: buttonUpdateGfwList
            text: qsTr("Update GFW List Now")
            contentItem: Text {
                id: buttonUpdateGfwListContentItem
                text: parent.text
                color: "#3498db"
                font.pointSize: 10.5
            }
            background: Rectangle {
                color: "#2e3e4e"
                radius: 4
            }
            onClicked: function() {
              buttonUpdateGfwList.enabled = false
              buttonUpdateGfwListContentItem.color = "white"
              buttonUpdateGfwList.text = qsTr("Updating ...")
              AppProxy.updateGfwList()
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
            labelGfwLastUpdatedTime.text = config["gfwListLastUpdated"]
        }

        function onAppConfigError(errorMsg) {
            labelErrorMsg.text = errorMsg
            labelErrorMsg.visible = true
        }

        function onGfwListUpdated(updatedTime) {
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
