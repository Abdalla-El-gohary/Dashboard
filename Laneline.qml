import QtQuick
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3

Item {
    x:1250
    y:500
    /*Image {
        id: group
        source: "app_icons/Group.png"
    }*/
    Image {
        id: laneline
        source: "app_icons/laneline1.png"
        scale: 0.6
        x:-390
        y:-390

    }

    Image {
        id: vehicle
        y:vehicle.y + 80
        x: vehicle.x - 20
        source: "icon/lanevehivle.png"
        scale:.99
    }
    Image {
        id: farvehicle
        y:vehicle.y - 180
        x: farvehicle.x - 25
        source: "icon/lanevehivle.png"
        scale:.45
        opacity: .35

    }
    RowLayout {
        id: gearLayout
        x: -250
        y: -330  // Positioning right above the rectangle
        spacing: 30
        Text {
            id: r
            text: qsTr("R")
            font.family: "Montserrat"
            font.pointSize: 15
        }
        Text {
            id: p
            text: qsTr("P")
            font.family: "Montserrat"
            font.pointSize: 15
        }
        Text {
            id: n
            text: qsTr("N")
            font.family: "Montserrat"
            font.pointSize: 15
        }
        Text {
            id: d
            text: qsTr("D")
            font.family: "Montserrat"
            font.bold: true
            font.pointSize: 15
        }
    }
    Rectangle{
        id:laneSpace
        width:1920/2 -150
        height: 2
        x:-250
        y:-300
        color: "black"
    }
    Text {
        id: speed
        text: qsTr("160")
        anchors.horizontalCenter: laneSpace.horizontalCenter
        y:-400
        font.family: "Montserrat"  // Change to any available font
        font.pointSize: 45  // Adjust size
        font.bold: true  // Make it bold (optional)
    }
    Text {
        id: kmh
        text: qsTr("Kmh")
        font.family: "Montserrat"
        font.pointSize: 12
        anchors.top: speed.bottom
        anchors.horizontalCenter: speed.horizontalCenter
    }
    RowLayout{
        y: -330
        x:480
        spacing: 1
        Text {
            id: health
            text: qsTr("90%")

        }
        Image {
            id: battry
            source: "app_icons/battery"
        }

    }


    Image {
        id: tirepressure
        source: "app_icons/tirepressure.svg"
        y: -500
        x: -250
    }
    Image {
        id: teslalogo
        source: "app_icons/TeslaLogo.svg"
        y: -490
        x: 500
    }



}
