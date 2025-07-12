import QtQuick
import QtQuick.Controls

Item {
    x: 900
    y: 40

    property alias cameracapture: opencameracap
    // Property to track the selected profile
    property string selectedProfile: ""

    // Signal to send profile data to C++ backend
    signal profileCreated(string name, string password)

    // File path for the JSON file
    property string profilesFile: "profiles.json"

    // Function to load profiles from JSON file
    function loadProfiles() {
        try {
            var jsonData = backend.readFile(profilesFile)
            if (jsonData) {
                var profiles = JSON.parse(jsonData)
                profileModel.clear()
                for (var i = 0; i < profiles.length; i++) {
                    profileModel.append(profiles[i])
                }
                // Set selected profile to the first one if available
                if (profiles.length > 0) {
                    selectedProfile = profiles[0].name
                }
            } else {
                console.log("No profiles found in file:", profilesFile)
            }
        } catch (e) {
            console.log("Error loading profiles:", e)
        }
    }

    // Function to save profiles to JSON file
    function saveProfiles() {
        try {
            var profiles = []
            for (var i = 0; i < profileModel.count; i++) {
                profiles.push(profileModel.get(i))
            }
            var jsonData = JSON.stringify(profiles, null, 2)
            backend.writeFile(profilesFile, jsonData)
        } catch (e) {
            console.log("Error saving profiles:", e)
        }
    }

    Component.onCompleted: {
        loadProfiles() // Load profiles only on startup
    }

    Rectangle {
        height: (profileModel.count * 50) + 20
        width: 300
        color: "whitesmoke"
        opacity: 0.95
        radius: 10

        // Popup for entering new profile name and password
        Rectangle {
            id: inputPopup
            visible: false
            anchors.centerIn: parent
            width: 240
            height: 200
            color: "white"
            border.color: "gray"
            radius: 5
            z: 10

            Column {
                anchors.centerIn: parent
                spacing: 10
                width: 220

                Text {
                    text: "New Profile"
                    font.pixelSize: 14
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                TextField {
                    id: profileNameInput
                    width: parent.width
                    placeholderText: "Profile Name"
                    font.pixelSize: 16
                    focus: true
                }

                TextField {
                    id: profilePasswordInput
                    width: parent.width
                    placeholderText: "Password"
                    font.pixelSize: 16
                    echoMode: TextInput.Password
                }
                Button {
                    id:opencameracap
                    text: "Open Camera Capture"
                    width: parent.width
                    onClicked: {
                        backend.openCamera()
                        cameraV.visible = true
                    }
                }
                Text {
                    id: txtcapturedone
                    text: qsTr("capture is done ✔")
                    color: "green"
                    visible: false
                }

                Row {
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: 20

                    Button {
                        text: "OK"
                        onClicked: {
                            if (profileNameInput.text.trim() !== "" && profilePasswordInput.text.trim() !== "") {
                                profileModel.append({
                                    name: profileNameInput.text.trim(),
                                    password: profilePasswordInput.text.trim()
                                })
                                // Emit signal to C++ backend
                                backend.sendprofileDate(profileNameInput.text.trim(), profilePasswordInput.text.trim())
                                // Set selected profile if none is selected
                                if (selectedProfile === "") {
                                    selectedProfile = profileNameInput.text.trim()
                                }
                                saveProfiles() // Save profiles to file
                                inputPopup.visible = false
                                profileNameInput.text = ""
                                profilePasswordInput.text = ""

                                txtcapturedone.visible = false
                                opencameracap.visible = true
                            }
                        }
                    }
                    Button {
                        text: "Cancel"
                        onClicked: {
                            inputPopup.visible = false
                            profileNameInput.text = ""
                            profilePasswordInput.text = ""

                        }
                    }
                }
            }
        }

        Rectangle {
            width: parent.width
            height: 20
            color: "gray"
            opacity: 0.7

            Row {
                anchors.centerIn: parent
                spacing: 80

                Text {
                    text: "edit"
                    color: "white"
                    font.pixelSize: 16
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            inputPopup.visible = true
                            profileNameInput.focus = true
                        }
                    }
                }
                Text {
                    text: "Save"
                    color: "white"
                    font.pixelSize: 16
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            saveProfiles() // Save profiles when "Save" is clicked
                        }
                    }
                }
            }
        }

        ListView {
            y: 20
            width: parent.width
            height: profileModel.count * 50
            model: ListModel {
                id: profileModel
            }

            delegate: Rectangle {
                width: parent.width
                height: 50
                color: model.name === selectedProfile ? "#d3d3d3" : "transparent"
                radius: 5

                Text {
                    id: profileText
                    anchors.verticalCenter: parent.verticalCenter
                    x: 20
                    text: model.name
                    font.pixelSize: 16
                    color: "black"
                }

                Row {
                    id: rightControls
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: 10
                    spacing: 10

                    Text {
                        id: checkMark
                        anchors.verticalCenter: parent.verticalCenter
                        text: model.name === selectedProfile ? "✔" : ""
                        color: "green"
                        font.pixelSize: 16
                    }

                    Button {
                        id: deleteButton
                        text: "✖"
                        width: 30
                        height: 30
                        z: 2

                        background: Rectangle {
                            color: deleteButton.pressed ? "#ff6666" : "#f0f0f0"
                            radius: 5
                        }

                        onClicked: {
                            console.log("Delete button clicked for profile:", model.name, "at index:", index)
                            if (profileModel.count > 1) {
                                if (model.name === selectedProfile) {
                                    selectedProfile = profileModel.get(index === 0 ? 1 : 0).name
                                }
                                profileModel.remove(index, 1)
                                saveProfiles() // Save profiles after deletion
                            } else {
                                profileModel.remove(index, 1)
                                selectedProfile = ""
                                saveProfiles() // Save profiles after deletion
                            }
                        }
                    }
                }

                MouseArea {
                    id: profileMouseArea
                    anchors.left: parent.left
                    anchors.right: rightControls.left
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    z: 1

                    Rectangle {
                        anchors.fill: parent
                        color: profileMouseArea.pressed ? "#e0e0e0" : "transparent"
                        opacity: 0.5
                    }

                    onClicked: {
                        console.log("Selecting profile:", model.name, "at index:", index)
                        backend.userSelected(model.name)
                    }
                    Connections {
                        target: backend

                        onUserAuthorized:function(name){
                           selectedProfile = model.name
                            console.log("The User:"+name+" Authorized !!!")

                        }
                        /*
                        onUserTookProfileImages:function(){
                            console.log("log")
                        }
                        */

                    }
                }
            }
        }
    }
}
