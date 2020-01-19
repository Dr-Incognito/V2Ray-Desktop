import QtQuick 2.0
import QtQuick.Layouts 1.3


ColumnLayout {
    anchors.fill: parent
    anchors.margins: 10
    spacing: 20

    RowLayout {
        Image {
            source: "qrc:///images/icon-settings.svg"
            sourceSize.width: 40
            sourceSize.height: 40
        }

        Text {
            text: qsTr("Settings")
            color: "white"
            font.pointSize: 24
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
