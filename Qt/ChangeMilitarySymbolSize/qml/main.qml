
// Copyright 2016 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//

import QtQuick 2.6
import QtQuick.Controls 1.4
import Esri.ChangeMilitarySymbolSize 1.0

ChangeMilitarySymbolSize {
    width: 800
    height: 600

    property real scaleFactor: System.displayScaleFactor

    // Create MapQuickView here, and create its Map etc. in C++ code
    MapView {
        anchors.fill: parent
        objectName: "mapView"
        // set focus to enable keyboard navigation
        focus: true
    }

    Slider {
        id: slider
        value: 0

        minimumValue: 1  // optional (default INT_MIN)
        maximumValue: 800 // optional (default INT_MAX)
        stepSize: 1      // optional (default 1)
        width: 400 // * scaleFactor
    }
    Text {
        id: sliderValue
        text: slider.value.toFixed(0)

        anchors.top: slider.bottom
    }

    Button {
        id: cursorButton
        anchors {
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
            margins: 32 * scaleFactor
        }

        text: "show dictionary renderer"
        onClicked: btnUPressed()
    }

    Button {
        id: otherButton
        anchors {
            bottom: parent.bottom
            left: cursorButton.right
            margins: 32 * scaleFactor
        }

        text: "show uniquevalue renderer"
        onClicked: btnDPressed()
    }

    Button {
        id: thirdButton
        anchors {
            top: sliderValue.bottom
            left: sliderValue.left
            margins: 32 * scaleFactor
        }

        text: "apply resize"
        onClicked: btnSPressed(slider.value.toFixed(0))
    }

}
