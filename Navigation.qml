import QtQuick
import QtQuick 6.0
import QtQuick.Window 6.0
import QtQuick.Controls 6.0
import QtQuick.Layouts 6.0
import QtPositioning 6.0
import QtLocation 6.0

Item {
    id: root
    width: 1920/2
    height: 930
    visible: true
    x: 890

    // === Theme Properties ===
    readonly property color primaryColor: "#4a6cf7"
    readonly property color secondaryColor: "#3a5ce4"
    readonly property color darkBackground: "#f5f5f5"
    readonly property color cardBackground: "#ffffff"
    readonly property color textColor: "#333333"
    readonly property color mutedText: "#666666"
    readonly property color borderColor: "#e0e0e0"
    readonly property color errorColor: "#d32f2f"
    readonly property int animationDuration: 300

    // === State Properties ===
    property var currentRoute: null
    property var waypoints: []
    property bool isNavigating: false
    property string errorMessage: ""
    property bool isMapReady: false

    // === Simulated GPS Properties ===
    property bool simulateGps: true
    property var simulatedCoordinate: QtPositioning.coordinate(31.21155, 29.92386)
    property int currentPathIndex: 0 // Index of current position in route path
    property real stepDistance: 10 // Meters to move per step

    // === Timer for Navigation ===
    property Timer centerMapTimer: Timer {
        interval: 500
        repeat: true
        running: isNavigating && isMapReady
        onTriggered: {
            console.log("Center map timer triggered")
            if (simulateGps && simulatedCoordinate.isValid) {
                map.center = simulatedCoordinate
            }
        }
    }

    // === Timer for Simulated GPS Movement ===
    Timer {
        id: gpsSimulationTimer
        interval: 1000 // Update every second
        repeat: true
        running: simulateGps && isNavigating && currentRoute && currentRoute.path.length > 0
        onTriggered: {
            if (!currentRoute || !currentRoute.path || currentPathIndex >= currentRoute.path.length - 1) {
                logMessage("Route path ended or invalid", "INFO")
                gpsSimulationTimer.stop()
                isNavigating = false
                return
            }

            // Move to next coordinate in route path
            let nextIndex = currentPathIndex + 1
            let nextCoord = currentRoute.path[nextIndex]
            if (!nextCoord.isValid) {
                logMessage(`Invalid coordinate at path index ${nextIndex}`, "ERROR")
                gpsSimulationTimer.stop()
                return
            }

            // Calculate distance to next coordinate
            let distanceToNext = simulatedCoordinate.distanceTo(nextCoord)
            if (distanceToNext <= stepDistance) {
                // Move directly to next coordinate
                simulatedCoordinate = nextCoord
                currentPathIndex = nextIndex
            } else {
                // Interpolate toward next coordinate
                let factor = stepDistance / distanceToNext
                let newLat = simulatedCoordinate.latitude + (nextCoord.latitude - simulatedCoordinate.latitude) * factor
                let newLon = simulatedCoordinate.longitude + (nextCoord.longitude - simulatedCoordinate.longitude) * factor
                simulatedCoordinate = QtPositioning.coordinate(newLat, newLon)
            }

            logMessage(`Simulated position: ${simulatedCoordinate.latitude}, ${simulatedCoordinate.longitude}`, "INFO")

            // Update waypoints and recalculate route
            if (waypoints.length > 0) {
                waypoints[0] = simulatedCoordinate
                updateWaypointMarkers()
                if (waypoints.length >= 2) {
                    calculateRoute()
                }
            }
        }
    }

    // === Logging Utility ===
    function logMessage(message, level = "INFO") {
        console.log(`[${level}] ${new Date().toISOString()}: ${message}`)
    }

    // === Waypoint Marker Component ===
    Component {
        id: waypointMarker
        MapQuickItem {
            property int markerIndex: 0
            coordinate: QtPositioning.coordinate(0, 0)
            anchorPoint.x: 12
            anchorPoint.y: 12
            sourceItem: Rectangle {
                width: 24
                height: 24
                color: primaryColor
                border.color: textColor
                border.width: 2
                radius: 12
                Text {
                    anchors.centerIn: parent
                    text: markerIndex + 1
                    color: textColor
                    font.pixelSize: 12
                    font.bold: true
                }
            }
        }
    }

    // === Utility Functions ===
    function formatDistance(meters) {
        if (isNaN(meters) || meters === null) {
            logMessage("Invalid distance value", "WARN")
            return "---"
        }
        return meters >= 1000 ? `${(meters / 1000).toFixed(1)} km` : `${meters.toFixed(0)} m`
    }

    function formatTime(seconds) {
        if (isNaN(seconds) || seconds <= 0) {
            logMessage("Invalid time value", "WARN")
            return "---"
        }
        if (seconds < 60) return `${Math.round(seconds)} sec`
        const minutes = Math.floor(seconds / 60)
        if (minutes < 60) return `${minutes} min`
        const hours = Math.floor(minutes / 60)
        return `${hours} h ${minutes % 60} min`
    }

    // === Navigation Functions ===
    function setDestination(address) {
        if (!address.trim()) {
            errorMessage = "Please enter a valid address"
            logMessage("Empty address input", "WARN")
            return
        }
        logMessage(`Setting destination: ${address}`)
        errorMessage = ""
        geocodeModel.query = address
        geocodeModel.update()
    }

    function calculateRoute() {
        if (waypoints.length < 2) {
            logMessage("Insufficient waypoints for route calculation", "WARN")
            return
        }

        logMessage("Calculating new route")
        currentRoute = null
        map.clearMapItems()

        const routeQuery = Qt.createQmlObject('import QtLocation 6.0; RouteQuery {}', root)
        waypoints.forEach((coord, index) => {
            if (coord.isValid) {
                routeQuery.addWaypoint(coord)
            } else {
                logMessage(`Invalid waypoint at index ${index}`, "ERROR")
            }
        })
        routeQuery.travelModes = RouteQuery.CarTravel
        routeQuery.routeOptimizations = RouteQuery.FastestRoute
        routeModel.query = routeQuery
        routeModel.update()
    }

    function addWaypoint(coordinate, index = -1) {
        if (!coordinate.isValid) {
            logMessage("Attempted to add invalid coordinate", "ERROR")
            return
        }
        logMessage(`Adding waypoint at ${coordinate.latitude}, ${coordinate.longitude}`)
        if (index === -1) {
            waypoints.push(coordinate)
        } else {
            waypoints.splice(index, 0, coordinate)
        }
        updateWaypointMarkers()
        if (waypoints.length >= 2) {
            calculateRoute()
        }
    }

    function removeWaypoint(index) {
        if (index < 0 || index >= waypoints.length) {
            logMessage(`Invalid waypoint index: ${index}`, "ERROR")
            return
        }
        logMessage(`Removing waypoint at index ${index}`)
        waypoints.splice(index, 1)
        updateWaypointMarkers()
        if (waypoints.length >= 2) {
            calculateRoute()
        } else {
            clearRoute()
        }
    }

    function updateWaypointMarkers() {
        logMessage("Updating waypoint markers")
        map.clearMapItems()
        waypoints.forEach((coord, index) => {
            if (coord.isValid) {
                let marker = waypointMarker.createObject(map, {
                    coordinate: coord,
                    markerIndex: index
                })
                map.addMapItem(marker)
            }
        })
        if (currentRoute) {
            map.addRoute(currentRoute)
        }
        map.addMapItem(userPosition)
    }

    function clearRoute() {
        logMessage("Clearing current route")
        map.clearMapItems()
        currentRoute = null
        waypoints = simulateGps && simulatedCoordinate.isValid ? [simulatedCoordinate] : []
        isNavigating = false
        currentPathIndex = 0
        centerMapTimer.stop()
        gpsSimulationTimer.stop()
        errorMessage = ""
        map.addMapItem(userPosition)
        map.update()
    }

    // === Models ===
    RouteModel {
        id: routeModel
        plugin: map.plugin
        autoUpdate: false
        onStatusChanged: {
            if (status === RouteModel.Ready && count > 0) {
                logMessage("Route calculation successful")
                currentRoute = get(0)
                currentPathIndex = 0 // Reset path index for new route
                map.clearMapItems()
                updateWaypointMarkers()
                map.addRoute(currentRoute)
                routeInfoCard.updateRouteInfo()
                errorMessage = ""
            } else if (status === RouteModel.Error) {
                logMessage("Route calculation failed", "ERROR")
                errorMessage = "Failed to calculate route"
            }
        }
    }

    GeocodeModel {
        id: geocodeModel
        plugin: Plugin { name: "osm" }
        onStatusChanged: {
            if (status === GeocodeModel.Ready && count > 0) {
                logMessage("Geocoding successful")
                const dest = get(0).coordinate
                waypoints = [simulateGps ? simulatedCoordinate : QtPositioning.coordinate(31.21155, 29.92386)]
                addWaypoint(dest)
                errorMessage = ""
            } else if (status === GeocodeModel.Error) {
                logMessage("Geocoding failed", "ERROR")
                errorMessage = "Failed to geocode address"
            }
        }
    }

    // === UI Components ===
    Rectangle {
        anchors.fill: parent
        color: darkBackground

        // === Header Section ===
        Rectangle {
            id: header
            width: parent.width
            height: 80
            color: "transparent"

            RowLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 20

                Text {
                    text: "NAVIGATION"
                    font.pixelSize: 24
                    font.bold: true
                    color: primaryColor
                }

                Rectangle {
                    Layout.fillWidth: true
                    height: 48
                    radius: 24
                    color: cardBackground
                    border.color: borderColor

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 12
                        Text {
                            text: "📍 "
                            font.pixelSize: 16
                            color: mutedText
                        }
                        Text {
                            text: simulateGps && simulatedCoordinate.isValid ?
                                  `${simulatedCoordinate.latitude.toFixed(5)}, ${simulatedCoordinate.longitude.toFixed(5)}` :
                                  "Acquiring..."
                            font.pixelSize: 14
                            color: textColor
                            Layout.fillWidth: true
                        }
                    }
                }

                Rectangle {
                    width: 220
                    height: 48
                    radius: 24
                    color: cardBackground
                    border.color: borderColor
                    Text {
                        anchors.centerIn: parent
                        text: Qt.formatDateTime(new Date(), "hh:mm AP • ddd, MMM d")
                        font.pixelSize: 14
                        color: textColor
                    }
                }
            }
        }

        // === Main Content Section ===
        RowLayout {
            anchors.top: header.bottom
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            spacing: 0

            // === Left Control Panel ===
            Rectangle {
                id: leftPanel
                Layout.preferredWidth: 280
                Layout.fillHeight: true
                color: "transparent"

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 20

                    // === Search Card ===
                    Rectangle {
                        id: searchCard
                        Layout.fillWidth: true
                        height: 120
                        radius: 16
                        color: cardBackground
                        border.color: borderColor

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 16
                            spacing: 12

                            Text {
                                text: "Destination"
                                font.pixelSize: 16
                                font.bold: true
                                color: textColor
                            }

                            RowLayout {
                                Layout.fillWidth: true
                                spacing: 10

                                TextField {
                                    id: addressInput
                                    Layout.fillWidth: true
                                    font.pixelSize: 16
                                    placeholderText: "Enter address or place"
                                    color: textColor
                                    background: Rectangle {
                                        color: "#f0f0f0"
                                        radius: 12
                                        border.color: addressInput.activeFocus ? primaryColor : borderColor
                                        border.width: 1
                                    }
                                    onAccepted: goButton.clicked()
                                }

                                Button {
                                    id: goButton
                                    text: "GO"
                                    font.pixelSize: 14
                                    font.bold: true
                                    implicitWidth: 60
                                    implicitHeight: 40
                                    enabled: addressInput.text.trim() !== ""
                                    background: Rectangle {
                                        radius: 12
                                        color: goButton.down ? secondaryColor : primaryColor
                                        opacity: goButton.enabled ? 1.0 : 0.5
                                    }
                                    contentItem: Text {
                                        text: goButton.text
                                        font: goButton.font
                                        color: textColor
                                        horizontalAlignment: Text.AlignHCenter
                                        verticalAlignment: Text.AlignVCenter
                                    }
                                    onClicked: setDestination(addressInput.text)
                                }
                            }
                        }
                    }

                    // === Route Information Card ===
                    Rectangle {
                        id: routeInfoCard
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        radius: 16
                        color: cardBackground
                        border.color: borderColor

                        function updateRouteInfo() {
                            if (currentRoute && currentRoute.path.length > 0) {
                                logMessage("Updating route information")
                            }
                        }

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 16
                            spacing: 12

                            Text {
                                text: errorMessage || "Route Information"
                                font.pixelSize: 16
                                font.bold: true
                                color: errorMessage ? errorColor : textColor
                                Layout.fillWidth: true
                                elide: Text.ElideRight
                            }

                            ScrollView {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                visible: currentRoute !== null
                                clip: true

                                ColumnLayout {
                                    width: parent.width
                                    spacing: 8

                                    RowLayout {
                                        spacing: 10
                                        Text { text: "Distance:"; font.pixelSize: 14; color: mutedText }
                                        Text {
                                            text: currentRoute ? formatDistance(currentRoute.distance) : "---"
                                            font.pixelSize: 14
                                            font.bold: true
                                            color: textColor
                                        }
                                    }

                                    RowLayout {
                                        spacing: 10
                                        Text { text: "Time:"; font.pixelSize: 14; color: mutedText }
                                        Text {
                                            text: currentRoute ? formatTime(currentRoute.travelTime) : "---"
                                            font.pixelSize: 14
                                            font.bold: true
                                            color: textColor
                                        }
                                    }
                                }
                            }

                            Text {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                text: "No route information available\nSet a destination to see route details"
                                color: mutedText
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignHCenter
                                visible: currentRoute === null && !errorMessage
                                wrapMode: Text.WordWrap
                            }
                        }
                    }
                }
            }

            // === Map Display Area ===
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "transparent"

                Map {
                    id: map
                    anchors.fill: parent
                    anchors.margins: 10
                    plugin: Plugin {
                        name: "osm"
                        parameters: [
                            PluginParameter { name: "osm.mapping.providersrepository.disabled"; value: "true" },
                            PluginParameter { name: "osm.mapping.apikey"; value: "4fbb2ec443a04348a535331ceca9be93" },
                            PluginParameter { name: "osm.mapping.provider"; value: "https://tile.thunderforest.com/transport/{z}/{x}/{y}.png" }
                        ]
                    }
                    center: simulateGps && simulatedCoordinate.isValid ?
                            simulatedCoordinate :
                            QtPositioning.coordinate(31.21155, 29.92386)
                    zoomLevel: 14
                    onMapReadyChanged: {
                        isMapReady = mapReady
                        logMessage(`Map ready state changed: ${isMapReady}`)
                    }

                    function addRoute(route) {
                        if (!route) {
                            logMessage("Attempted to add null route", "ERROR")
                            return
                        }
                        logMessage("Adding route to map")
                        const routeItem = Qt.createQmlObject('
                            import QtLocation 6.0;
                            MapRoute {
                                property var routeData
                                route: routeData
                                line.color: "#4a6cf7"
                                line.width: 5
                                smooth: true
                                opacity: 0.8
                            }', map)
                        routeItem.routeData = route
                        map.addMapItem(routeItem)
                    }

                    function clearMapItems() {
                        logMessage(`Clearing ${mapItems.length} map items`)
                        for (var i = mapItems.length - 1; i >= 0; i--) {
                            var item = mapItems[i]
                            if (item !== userPosition) {
                                map.removeMapItem(item)
                                item.destroy()
                            }
                        }
                    }

                    MapQuickItem {
                        id: userPosition
                        coordinate: simulateGps ? simulatedCoordinate : QtPositioning.coordinate(31.21155, 29.92386)
                        visible: coordinate.isValid
                        anchorPoint.x: 12
                        anchorPoint.y: 12
                        sourceItem: Rectangle {
                            width: 24
                            height: 24
                            color: primaryColor
                            border.color: textColor
                            border.width: 2
                            radius: 12
                        }
                    }
                }

                // === Map Controls ===
                Column {
                    anchors.right: parent.right
                    anchors.rightMargin: 20
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 10

                    Repeater {
                        model: [
                            { text: "+", action: () => map.zoomLevel = Math.min(map.zoomLevel + 1, map.maximumZoomLevel), enabled: isMapReady },
                            { text: "-", action: () => map.zoomLevel = Math.max(map.zoomLevel - 1, map.minimumZoomLevel), enabled: isMapReady },
                            { text: "📍",
                              visible: simulateGps && simulatedCoordinate.isValid,
                              action: () => map.center = simulatedCoordinate,
                              enabled: isMapReady },
                            { text: isNavigating ? "■" : "▶", visible: currentRoute !== null,
                              action: () => {
                                  isNavigating = !isNavigating
                                  logMessage(`Navigation state changed: ${isNavigating}`)
                                  if (isNavigating && waypoints.length >= 2 && currentRoute) {
                                      gpsSimulationTimer.start()
                                      centerMapTimer.start()
                                      currentPathIndex = 0 // Start at beginning of route
                                      simulatedCoordinate = currentRoute.path[0] // Align with route start
                                      waypoints[0] = simulatedCoordinate
                                      updateWaypointMarkers()
                                      calculateRoute()
                                  } else {
                                      gpsSimulationTimer.stop()
                                      centerMapTimer.stop()
                                  }
                              }, enabled: isMapReady },
                            { text: "✕",
                              visible: currentRoute !== null,
                              action: () => clearRoute(),
                              enabled: isMapReady }
                        ]

                        RoundButton {
                            width: 48
                            height: 48
                            visible: modelData.visible !== false
                            enabled: modelData.enabled !== false
                            background: Rectangle {
                                radius: parent.radius
                                color: parent.down ? Qt.darker(cardBackground, 1.2) : cardBackground
                                border.color: borderColor
                                opacity: parent.enabled ? 1.0 : 0.5
                            }
                            contentItem: Text {
                                text: modelData.text
                                font.pixelSize: 20
                                color: textColor
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            onClicked: modelData.action()
                        }
                    }
                }
            }
        }
    }

    // === Initialization ===
    Component.onCompleted: {
        if (simulateGps) {
            waypoints = [simulatedCoordinate]
            updateWaypointMarkers()
        }
        logMessage("Application initialized")
    }
}
