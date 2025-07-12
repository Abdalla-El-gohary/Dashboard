import QtQuick
import QtQuick.Controls 2.15
import QtLocation
import QtPositioning
Window {
    id:mainwin
    width: screen.width
    height: screen.height
    visible: true
    title: qsTr("Tesla dashboard")
    color: "gainsboro"
    //color: "lightgrey"
    MouseArea{
        anchors.fill: parent
        onClicked: {
            morewin.visible = false
            profilewin.visible = false
        }
    }


    Rectangle{
        id:backwhite
        width: 1920
        height: 1200
        color: "white"
    }
    Background{
        id:background
    }

    Image {
        id: sidebackground
        visible: false
        source: `icon/sidebar-light.png`

    }


    Footer{

    }

    Header{

    }

    Profile{
        id:profilewin
        visible: false
    }
    Navigation{
        id:navMap
        //anchors.fill: parent
        visible: false
    }
    FeatureToggle{
        id:mo
    }
    Laneline{
        id:laneGroub
        visible: false
    }

    Fota{

        id:fotaupdate
        visible: false
    }

    BootScreen{
        id : boot
        visible: false
    }


    Cameraview{
        id:cameraV
        visible: false
        anchors.centerIn: parent
    }



    MoreWin{
        id:morewin
    }




}
