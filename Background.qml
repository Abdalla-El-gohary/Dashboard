import QtQuick
import QtQuick.Controls 2.15
Item {



    Image {

        source: `icon/animate/${slider.value.toFixed(0)}.png`
        x:600
        y:450
        scale:1.5
        Slider {
            id: slider
            width: 500
            height: 300
            anchors.horizontalCenter: mainwin.horizontalCenter
            from: 1 // Start from 1.png
            to: 50  // End at 50.png
            stepSize: 1 // Increment by 1
            value: 1   // Initial value
            opacity: 0
        }

    }
}
