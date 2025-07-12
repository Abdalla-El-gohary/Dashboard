import QtQuick
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

Item {
    y:10
    x:1100


    Image {
        //visible: false
        id: foo
        source: `icon/animate/top/${slider7.value.toFixed(0)}.png`

        scale:1
        Slider {
            id: slider7
            x:200
            y:-100
            opacity: 0
            width: 250
            height: 500
            from: 24 // Start from 1.png
            to: 1  // End at 50.png
            stepSize: -1 // Increment by 1
            value: 24   // Initial value
            rotation: 90

        }

    }

    Rectangle{
        width: 1920 / 3
        height: 1
        color: "black"
        anchors.top:foo.bottom

    }
    ColumnLayout{
        y:400
        RowLayout{
            spacing: 500
            Text {
                id: software
                text: qsTr("software")
                font.family: "Montserrat"
                font.pointSize: 13
                color: "black"
                font.bold: true

            }
            Text {
                id: relasenote
                text: qsTr("Release Notes")
                font.family: "Montserrat"
                font.pointSize: 12
                color: "blue"
            }
        }
        Text {
            id: version
            text: qsTr("V.0.0( 2025.0.0.efee7eff7 )")
            font.family: "Montserrat"
            font.pointSize: 12
            color: "black"
        }

    }


    ColumnLayout{
        y:500

        RowLayout{
            spacing: 450
            Text {

                id: update
                text: qsTr("update avilable")
                font.family: "Montserrat"
                font.pointSize: 13
                color: "black"
                font.bold: true
            }
            Text {
                id: relasenoteNEW
                text: qsTr("Release Notes")
                font.family: "Montserrat"
                font.pointSize: 12
                color: "blue"

                MouseArea{
                    anchors.fill: parent
                    onClicked: {

                        backend.sayHello()
                    }
                }
            }

        }


        Text {
            id: newupdate
            text: backend.paragraphText
            font.family: "Montserrat"
            font.pointSize: 12
            color: "black"
        }

    }
    RowLayout{
        spacing: 200
        y:700

        Button {
            id:search_update_button
            text: qsTr("Search")
            width: 200
            height: 60
            visible: backend.searchState
            onClicked: {
                //call Function that fetch the update notes
                backend.fetchUpdates();

            }
        }
        Button {
            id:update_button
            text: qsTr("update")
            width: 200
            height: 60
            visible: backend.updateState
            onClicked: {
                //call Function that updates the target
                backend.installUpdate();

                //boot.visible = true;
            }
        }

        Button {
            text: qsTr("reboot")
            width: 200
            height: 60
            visible: backend.rebootState
            onClicked: {
                boot.visible = true
                backend.reboot()
            }
        }
    }




}
