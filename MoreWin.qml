import QtQuick
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import QtQuick.Effects

Item {
    id: windowItem
    x: 510
    y: 300
    visible: false
    property bool accon: ture
    property bool afson: ture

    // Animation for smooth appearance
    onVisibleChanged: {
        if (visible) {
            root.opacity = 0 // Start with invisible
            root.scale = 0.8 // Start slightly smaller
            appearAnimation.start()
        } else {
            root.opacity = 0 // Ensure it fades out when hidden
        }
    }

    Rectangle {
        id: root
        width: 900
        height: 450
        color: "black"
        opacity: 0 // Initial opacity set to 0
        scale: 0.8 // Initial scale for animation
        property string activeWindow: "main"

        // Animation for smooth appearance
        SequentialAnimation {
            id: appearAnimation
            NumberAnimation {
                target: root
                property: "opacity"
                from: 0
                to: 0.8
                duration: 650 // Duration in milliseconds
                easing.type: Easing.InOutQuad // Smooth easing
            }
            NumberAnimation {
                target: root
                property: "scale"
                from: 0.8
                to: 1.0
                duration: 300 // Duration in milliseconds
                easing.type: Easing.InOutQuad // Smooth easing
            }
        }

        RowLayout {
            x: -100
            y: -100
            spacing: -200
            Image {
                id: acc
                source: "app_icons/ACCicon.png"
                scale: 0.2

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        //accText.visible = !accText.visible

                        accon = !accon
                        mo.adaptiveIcon.opacity = accon? 1:.1


                    }
                }

                Text {
                    id: accText
                    text: qsTr("ACC mode")
                    color: "white"
                    scale: 5
                    anchors.top: acc.bottom
                    anchors.horizontalCenter: acc.horizontalCenter
                    anchors.topMargin: 50
                }
            }
            Image {
                id: fota
                source: "app_icons/FOTA.png"
                scale: 0.2

                Text {
                    id: fotaText
                    text: qsTr("new update")
                    color: "white"
                    scale: 5
                    anchors.top: fota.bottom
                    anchors.horizontalCenter: fota.horizontalCenter
                    anchors.topMargin: 50
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {

                        if (root.activeWindow === "fota") {
                            profilewin.x = 900
                            root.activeWindow = "main"
                            fotaupdate.visible = false
                            laneGroub.visible = false
                            navMap.visible = false
                            backwhite.width = 1920
                            morewin.visible = true
                            background.scale = 1
                            background.y = 0
                            background.x = 0
                        } else {
                            profilewin.x = 300
                            root.activeWindow = "fota"
                            fotaupdate.visible = true
                            laneGroub.visible = false
                            navMap.visible = false
                            backwhite.width = 1920 / 2
                            morewin.visible = false
                            background.scale = 0.7
                            background.y = 100
                            background.x = -150
                            //backFOTA.op
                        }
                    }
                }
            }
            Image {
                id: lanekeep
                source: "app_icons/LaneIcon.png"
                scale: 0.2

                Text {
                    id: lanekeepText
                    text: qsTr("Lane keep")
                    color: "white"
                    scale: 5
                    anchors.top: lanekeep.bottom
                    anchors.horizontalCenter: lanekeep.horizontalCenter
                    anchors.topMargin: 50
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (root.activeWindow === "lanekeep") {
                            profilewin.x = 900
                            root.activeWindow = "main"
                            fotaupdate.visible = false
                            laneGroub.visible = false
                            navMap.visible = false
                            backwhite.width = 1920
                            morewin.visible = true
                            background.scale = 1
                            background.y = 0
                            background.x = 0
                            mo.laneIcon.opacity = .1
                        } else {
                            profilewin.x = 300
                            root.activeWindow = "lanekeep"
                            fotaupdate.visible = false
                            laneGroub.visible = true
                            navMap.visible = false
                            backwhite.width = 1920 / 2
                            morewin.visible = false
                            background.scale = 0.7
                            background.y = 100
                            background.x = -150
                            mo.laneIcon.opacity = 1
                        }
                    }
                }
            }
            Image {
                id: afs
                source: "app_icons/AFS.png"
                scale: 0.2

                Text {
                    id: afsText
                    text: qsTr("AFS light")
                    color: "white"
                    scale: 5
                    anchors.top: afs.bottom
                    anchors.horizontalCenter: afs.horizontalCenter
                    anchors.topMargin: 50
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                       // afsText.visible = !afsText.visible
                        afson = !afson
                        mo.afsIcon.opacity = afson? 1:.1
                    }
                }
            }
            Image {
                id: nav
                //source: "icon/Navication"
                source: "app_icons/navigate.png"
                scale: .2

                Text {
                    id: navText
                    text: qsTr("navigation")
                    color: "white"
                    scale: 5
                    anchors.top: nav.bottom
                    anchors.horizontalCenter: nav.horizontalCenter
                    anchors.topMargin: 50
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (root.activeWindow === "navigation") {
                            profilewin.x = 900
                            root.activeWindow = "main"
                            navMap.visible = false
                            fotaupdate.visible = false
                            laneGroub.visible = false

                            backwhite.width = 1920
                            morewin.visible = true
                            background.scale = 1
                            background.y = 0
                            background.x = 0
                            mo.gpsIcon.opacity = .1
                        } else {
                            root.activeWindow = "navigation"
                            profilewin.x = 300
                            navMap.visible = true
                            fotaupdate.visible = false
                            laneGroub.visible = false
                            backwhite.width = 1920 / 2
                            morewin.visible = false
                            background.scale = 0.7
                            background.y = 100
                            background.x = -150
                            mo.gpsIcon.opacity = 1
                        }
                    }
                }
            }
        }
    }
}
