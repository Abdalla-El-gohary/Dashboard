import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

Item {
    width: 800
    height: 600
    property string captureState: "capturingFront" // capturingFront, capturingLeft, capturingRight, errorFront, errorLeft, errorRight, done
    property bool userSelected: false
    Rectangle {
        id: container
        anchors.fill: parent
        color: "#f0f0f0"
        radius: 10
        border.color: "#cccccc"
        border.width: 1

        Image {
            id: profileImage
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                margins: 10
            }
            height: parent.height - 100
            fillMode: Image.PreserveAspectFit
            source: backend.frameUrl || ""
            cache: false

            Text {
                anchors.centerIn: parent
                text: "Failed to load image"
                color: "red"
                visible: profileImage.status === Image.Error
            }

            Text {
                anchors.centerIn: parent
                text: "Loading..."
                color: "gray"
                visible: profileImage.status === Image.Loading
            }
        }

        RowLayout {
            visible: !userSelected
            anchors {
                bottom: parent.bottom
                horizontalCenter: parent.horizontalCenter
                bottomMargin: 20
            }
            spacing: 20

            Button {
                text: "Cancel"
                visible: true
                onClicked: {
                    console.log("Cancelled")
                    captureState = "capturingFront"
                    cameraV.visible = false
                    backend.captureCanceled();

                }
            }

            Button {
                text: "Try Again"
                visible: captureState === "errorFront" || captureState === "errorLeft" || captureState === "errorRight"
                onClicked: {
                    console.log("Retake current angle: " + captureState)
                    // Refresh the image based on the error state
                    if (captureState === "errorFront") {
                        backend.captureFront()
                        captureState = "capturingFront"
                    } else if (captureState === "errorLeft") {
                        backend.captureLeft()
                        captureState = "capturingLeft"
                    } else if (captureState === "errorRight") {
                        backend.captureRight()
                        captureState = "capturingRight"
                    }
                }
            }


            Button {
                text: "Confirm"
                visible: captureState === "capturingFront" || captureState === "capturingLeft" || captureState === "capturingRight"

                onClicked: {
                    if (captureState === "capturingFront") {
                        backend.captureFront()
                    } else if (captureState === "capturingLeft") {
                        backend.captureLeft()
                    } else if (captureState === "capturingRight") {
                        backend.captureRight()
                    }
                }
            }

            // Connect to result from backend
            Connections {
                target: backend

                onCurrentUserChanged: function(name) {
                    console.log("User selected:", name)
                    userSelected = true
                    cameraV.visible = true
                    captureState = "idle"
                }
                onCaptureFrontResult: function(success) {
                    if (captureState !== "capturingFront") return  // ensure correct state

                    if (success){
                        captureState = "capturingLeft"
                        console.log("capture right")
                    }else
                        captureState = "errorFront"
                }
                onUserAuthorized:function(name){
                    cameraV.visible = false
                }

                onCaptureLeftResult: function(success) {
                    if (captureState !== "capturingLeft") return

                    if (success){
                        console.log("capture left")
                        captureState = "capturingRight"
                    }
                    else
                        captureState = "errorLeft"
                }

                onCaptureRightResult: function(success) {
                    if (captureState !== "capturingRight") return

                    if (success) {
                        captureState = "done"
                        profilewin.cameracapture.visible = false
                        profilewin.txtcapturedone.visible = true
                    } else {
                        captureState = "errorRight"
                    }
                }
            }
        }
        Text {
            id: stateText
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
                bottomMargin: 60
            }
            text: {
                switch (captureState) {
                case "capturingFront": return "Capturing Front View"
                case "capturingLeft": return "Capturing Left View"
                case "capturingRight": return "Capturing Right View"
                case "errorFront": return "Error Capturing Front - Try Again"
                case "errorLeft": return "Error Capturing Left - Try Again"
                case "errorRight": return "Error Capturing Right - Try Again"
                case "done": return "Capture complete!"
                default: return ""
                }
            }
            color: captureState.startsWith("error") ? "red" : captureState === "done" ? "green" : "black"
            font.bold: captureState === "done" || captureState.startsWith("error")
            visible: true
        }
    }
}
