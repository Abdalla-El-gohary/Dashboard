cmake_minimum_required(VERSION 3.16)

project(tesla_dashboard VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Required Qt modules
find_package(Qt6 6.5 REQUIRED COMPONENTS Concurrent Quick Svg Core Gui Widgets)

# Set up project structure
qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(apptesla_dashboard
    main.cpp
)

file(GLOB_RECURSE ANIMATE_ICONS
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    icon/animate/*.png)

qt_add_qml_module(apptesla_dashboard
    URI tesla_dashboard
    VERSION 1.0
    QML_FILES
        Main.qml
        Footer.qml
        Header.qml
        MoreWin.qml
        Laneline.qml
        Fota.qml
        Background.qml
        FOTAnotes.qml
        BootScreen.qml
        Profile.qml
        Navigation.qml
        Cameraview.qml
    RESOURCES
        # App icons
        app_icons/ACCicon.png
        app_icons/AFS.png
        app_icons/bluetooth.svg
        app_icons/calendar.svg
        app_icons/caraoke.svg
        app_icons/dashcam.svg
        app_icons/FOTA.png
        app_icons/front-defrost.svg
        app_icons/LaneIcon.png
        app_icons/laneline.svg
        app_icons/messages.svg
        app_icons/model-3.svg
        app_icons/music.svg
        app_icons/phone.svg
        app_icons/radio.svg
        app_icons/rear-defrost.svg
        app_icons/recording.svg
        app_icons/seat-warmer.svg
        app_icons/seat.svg
        app_icons/spotify.svg
        app_icons/steering-wheel-warmer.svg
        app_icons/toybox.svg
        app_icons/tunein.svg
        app_icons/video.svg
        app_icons/volume.svg
        app_icons/wiper.svg
        app_icons/zoom.svg
        app_icons/Group.png
        app_icons/laneline1.png
        app_icons/battery.svg
        app_icons/SpeedLimit.svg
        app_icons/steeringwheel.svg
        app_icons/TeslaLogo.svg
        app_icons/icons8-tesla-50.png
        app_icons/tesla_logo.png
        app_icons/tesla_name.png
        app_icons/navigation.png

        # Top header icons
        top_header_icons/dark/icons.svg
        top_header_icons/dark/lock.svg
        top_header_icons/dark/Sentry.svg
        top_header_icons/airbag_.svg
        top_header_icons/battery.svg
        top_header_icons/icons.svg
        top_header_icons/lock.svg
        top_header_icons/Sentry.svg

        # Other icons
        icon/headlights.png
        icon/lanevehivle.png
        icon/vehicleLane.svg
        icon/lightbackground
        icon/sidebar-light.png
        # icons for features on and off
        icon/mic_off.png
        icon/mic_on.png
        icon/traffic_on.png
        icon/traffic_off.png

        # Stepper icons
        icon/stepper_icons/left-arrow.svg
        icon/stepper_icons/right-arrow.svg

        ${ANIMATE_ICONS}
        SOURCES commmanager.h commmanager.cpp
        SOURCES zmqsubscriber.h zmqsubscriber.cpp
        SOURCES
        SOURCES commconfig.h
        QML_FILES FeatureToggle.qml
        SOURCES containerworker.h containerworker.cpp
        RESOURCES icon/Navication.png
        RESOURCES icon/adaptiveCuris.png icon/gps.png icon/keep-in-lane.png
        RESOURCES icon/AFS.png
        RESOURCES app_icons/navigate.png
)

# Source files
target_sources(apptesla_dashboard PRIVATE
    backend.h backend.cpp

    zmqclient.h zmqclient.cpp


    manifestmanager.h manifestmanager.cpp
    networkmanager.h networkmanager.cpp
    updater.h updater.cpp
    backgroundworker.h backgroundworker.cpp
)

# Application properties
set_target_properties(apptesla_dashboard PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

# OpenCV
find_package(OpenCV REQUIRED PATHS "/usr")
include_directories("/usr/include/opencv4")

# Link libraries
target_link_libraries(apptesla_dashboard PRIVATE
    Qt6::Quick
    Qt6::Svg
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Concurrent
    ${OpenCV_LIBS}
    zmq
)

# Installation rules
include(GNUInstallDirs)
install(TARGETS apptesla_dashboard
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
