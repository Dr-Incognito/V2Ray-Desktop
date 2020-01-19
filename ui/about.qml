import QtQuick 2.3
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
        columns: 2
        flow: GridLayout.LeftToRight

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

        Label {
            text: qsTr("Project Page")
            color: "white"
            font.bold: true
        }

        Label {
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
