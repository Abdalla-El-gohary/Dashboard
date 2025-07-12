import QtQuick
import QtQuick.Layouts 1.3
Item {


    Rectangle{
        width: 1920
        height: 1200
        color: "black"


        Image {
            x:500
            y:0
            id: tesla_logo
            source: "app_icons/tesla_logo.png"
            scale: .2
        }

        Image {
            x:500
            y:450
            id: tesla_Name
            source: "app_icons/tesla_name.png"
            scale: .2
        }

        Rectangle{
            height: 3
            width: 400
            color: "gray"
            x:700
            y:530
            opacity: .7
        }
        Rectangle {
            id: loadingBar
            x: 700
            y: 530 // Positioned just below the background bar
            width: backend.bootwidth // Starts at 0, grows to 400
            height: 6
            color: "white" // Change to desired color

            // Animation to simulate loading
            /*PropertyAnimation {
                id: loadingAnimation
                target: loadingBar
                property: "width"
                from: 0
                to: 400
                duration: 10000 // 3 seconds for full loading
                easing.type: Easing.InOutQuad
                running: true // Start animation on load
                loops: Animation.Infinite // Repeat for demo
            }
            */
        }

    }



}
