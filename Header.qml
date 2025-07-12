import QtQuick
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
Item {

    id:header
    anchors.top: parent.top
    x:backwhite.width/2 -150
    RowLayout{
        spacing: 32

        Image {
            id: lock
            source: "top_header_icons/lock.svg"
        }

        Image {
            id: profile
            source: "top_header_icons/icons.svg"
        }
        Text {
            id: profileName
            text: profilewin.selectedProfile
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    profilewin.visible = !profilewin.visible
                }
            }

        }

        Image {
            id: sentry
            source: "top_header_icons/Sentry.svg"
        }

    }

}
