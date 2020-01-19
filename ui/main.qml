import QtQuick 2.12
import QtQuick.Window 2.12

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
}
