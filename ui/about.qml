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
            source: "qrc:///images/icon-about.svg"
            sourceSize.width: 40
            sourceSize.height: 40
            mipmap: true
        }

        Text {
            text: qsTr("About")
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
            text: qsTr("V2Ray Desktop Version")
            color: "white"
            font.bold: true
        }

        Label {
            id: textAppVersion
            text: "N/a"
            color: "white"
        }

        Button {
            text: qsTr("Check for Updates")
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

        Label {
            text: qsTr("V2Ray Core Version")
            color: "white"
            font.bold: true
        }

        Label {
            id: textV2rayVersion
            text: "N/a"
            color: "white"
        }

        Button {
            text: qsTr("Check for Updates")
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

        Label {
            text: qsTr("Project Page")
            color: "white"
            font.bold: true
        }

        Text {
            text: "https://github.com/Dr-Incognito/V2Ray-Desktop"
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
}
