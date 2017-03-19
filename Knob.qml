import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

Control {
    id: root

    property var computedImplicitWidth: fontMetrics.boundingRect(ccText.text).width + grid.columnSpacing + mySpinBox.implicitWidth
    property var computedImplicitHeight: title.implicitHeight + mySpinBox.implicitHeight + grid.rowSpacing
//    property var computedImplicitDimentsion: Math.max(computedImplicitWidth, computedImplicitHeight)

    implicitWidth: computedImplicitWidth
    implicitHeight: computedImplicitHeight

    Rectangle {
        anchors.fill: root
        color: "transparent"
        border.color: "red"
        border.width: 1
        z: 10
    }

    Rectangle {
        anchors.fill: grid
        color: "lightgreen"
    }

    FontMetrics {
        id: fontMetrics
        font: ccText.font
    }

    GridLayout {
        id: grid

        anchors.fill: root
        columns: 2

        Text {
            id: title

            Layout.fillWidth: true
            Layout.columnSpan: 2

            text: presetId + " / " + programId + " / " + controlId
        }

        Text {
            id: ccText
            text: "CC"
        }
        SpinBox {
            id: mySpinBox
            editable: true
            Layout.fillWidth: true;
            to: 127
            value: 127
        }
    }
}