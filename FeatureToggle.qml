import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: featureToggle

    x:50
    y:150

    property alias adaptiveIcon: backAdaptiveCuris
    property alias laneIcon: backLaneKeep
    property alias gpsIcon: backNavigation
    property alias afsIcon: backAFS

    Column {
        scale: .5
        spacing: 16
        anchors.centerIn: parent

        Image {
            id:backAdaptiveCuris
            source: "icon/adaptiveCuris"
            width: 100
            height: 100
            fillMode: Image.PreserveAspectFit
            opacity: .1
        }

        Image {
            id:backNavigation
            source: "icon/gps"
            width: 100
            height: 100
            fillMode: Image.PreserveAspectFit
            opacity: .1

        }

        Image {
            id:backLaneKeep
            source: "icon/keep-in-lane"
            width: 100
            height: 100
            fillMode: Image.PreserveAspectFit
            opacity: .1

        }
        Image {
            id:backAFS
            source: "icon/AFS.png"
            width: 100
            height: 100
            fillMode: Image.PreserveAspectFit
            opacity: .1

        }

    }

}
