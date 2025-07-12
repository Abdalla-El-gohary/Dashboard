import QtQuick
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
Item {
    width: mainwin.width
    height: 80
    anchors.bottom: parent.bottom
    Rectangle {
        id:foot
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "black" }
            GradientStop { position: 1.5; color: "#99000000" } // Equivalent to Style.black60
        }

        RowLayout{
            anchors.verticalCenter: parent.verticalCenter
            spacing: 15
            x:parent.width/4 - 100
            Image {

                source: "icon/stepper_icons/right-arrow.svg"
            }
            Text {
                id: num0
                font.pixelSize: 32
                color: "white"
                text: qsTr("11")
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        morewin.visible=!morewin.visible
                    }
                }
            }
            Image {

                source: "icon/stepper_icons/left-arrow.svg"
            }
        }




        RowLayout{
            spacing: 20
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            Image {
                id: phone

                source: "app_icons/phone.svg"

            }


            Image {
                id: radio
                source: "app_icons/radio.svg"
            }
            Image {
                id: bluetooth
                source: "app_icons/bluetooth.svg"
            }
            Image {
                id: spotify

                source: "app_icons/spotify.svg"
            }
            Image {
                id: dashcam

                source: "app_icons/dashcam.svg"
            }
            Image {
                id: video

                source: "app_icons/video.svg"
            }

        }


        RowLayout{
            anchors.verticalCenter: parent.verticalCenter
            spacing: 10
            //x:1350
            x:parent.width - (parent.width/4)
            Image {

                source: "icon/stepper_icons/right-arrow.svg"
            }
            Text {
                id: num
                color: "white"
                font.pixelSize: 32
                text: qsTr("11")
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        morewin.visible=!morewin.visible
                    }
                }
            }
            Image {

                source: "icon/stepper_icons/left-arrow.svg"
            }
        }

        Image {
            id: volume
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right

            source: "app_icons/volume.svg"
        }
        Image {
            id: car
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left

            source: "app_icons/model-3.svg"
        }

    }



}
