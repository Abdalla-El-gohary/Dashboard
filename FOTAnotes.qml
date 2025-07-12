import QtQuick
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3


Item {

    width: 300   // Default width
    height: 200  // Default height

    visible: false
    // Expose properties to customize size and position
    property alias rectangleColor: rectangle.color
    property alias paragraphText: paragraph.text

    Rectangle {
        id: rectangle
        anchors.fill: parent
        color: "lightblue" // Default color
        ColumnLayout{
            spacing: 20
            Text {
                id: updates
                text: qsTr("New updates in the next version.")
                anchors.top: parent.top
                font.bold: true
                font.pixelSize: 18
                color: "white"
            }
            Text {
                id: paragraph
                anchors.centerIn: parent
                width: parent.width * 0.8
                text: "ahhhhhhhhhhhh wg3 wla dl3"
                wrapMode: Text.WrapAnywhere
                font.pixelSize: 16
                color: "white"
            }
        }
    }
}
