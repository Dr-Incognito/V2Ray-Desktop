import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Layouts 1.3


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
            model: ListModel{
                ListElement { text: "PAC Mode" }
                ListElement { text: "Global Mode" }
                ListElement { text: "Manual Mode" }
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
            text: "https://raw.githubusercontent.com/gfwlist/gfwlist/master/gfwlist.txt"
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
            id: textGfwLastUpdatedTime
            Layout.fillWidth: true
            text: qsTr("Never")
            color: "white"
        }

        Button {
            text: qsTr("Update GFW List Now")
            contentItem: Text {
                text: parent.text
                color: "#3498db"
            }
            background: Rectangle {
                color: "#2e3e4e"
                radius: 4
            }
            onClicked: function() {
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
