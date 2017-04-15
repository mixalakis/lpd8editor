import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtQuick.Window 2.0

ApplicationWindow {
    visible: true
    minimumWidth: (presetsColumn.Layout.minimumWidth + padsColumn.Layout.minimumWidth) * 1.1 // 1.1 temp hack
    minimumHeight: Screen.height / 4
    title: qsTr("lpd8-editor") // XXX get applicationName

    property int globalSpacing: programButtons.spacing
    property alias padSize: invisiblePad.computedImplicitDimension
    property alias knobHeight: invisibleKnob.implicitHeight

//    MockupPadsModel {
//        id: padsModel
//    }

//    MockupKnobsModel {
//        id: knobsModel
//    }

    MockupProgramsModel {
        id: programsModel
    }

    Rectangle {
        anchors.fill: columns
        color: "darkgrey"
    }

    Pad {
        id: invisiblePad
        visible: false
    }

    Knob {
        id: invisibleKnob
        visible: false
    }

    RowLayout {
        id: columns

        anchors.fill: parent

        ColumnLayout {
            id: presetsColumn

            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.minimumWidth: Screen.width / 8

            Rectangle {
                anchors.fill: presetsColumn
                color: "steelblue"
            }

            RowLayout {
                Text {
                    Layout.fillWidth: true;

                    text: "Presets"
                }
                Button {
                    Layout.alignment: Qt.AlignRight
                    text: "Add"
                    onClicked: {
                        app.newPreset();
                    }
                }
            }

            Rectangle {
                anchors.fill: presetsSection
                color: "darkblue"
            }

            ListView {
                id: presetsSection
                Layout.fillHeight: true
                Layout.fillWidth: true

                spacing: globalSpacing

                ScrollBar.vertical: ScrollBar{}

                interactive: true

                model: presets
                delegate: Preset {
                    width: parent.width
                }
            }
        }

        ColumnLayout {
            id: padsColumn

            Layout.fillHeight: true
            Layout.fillWidth: false
            Layout.minimumWidth: 4 * (padSize+globalSpacing)
            Layout.alignment: Qt.AlignTop

            Rectangle {
                anchors.fill: padsColumn
                color: "lightgreen"
            }

            RowLayout {
                Text {
                    Layout.fillWidth: true
                    text: "Program"
                }

                RowLayout {
                    id: programButtons

                    Layout.fillWidth: false
                    Layout.alignment: Qt.AlignRight

                    Repeater {
                        model: programsModel
                        delegate: Button {
                            Layout.fillWidth: true

                            autoExclusive: true
                            checkable: true

                            checked: model.current
                            text: model.programId
                            onClicked: {
                                app.activeProgramId = model.programId
                            }
                        }
                    }
                }
            }

            Text {
                text: "Pads"
            }

            GridView {
                id: padsView

                Layout.fillWidth: true
                Layout.fillHeight: false
                Layout.minimumHeight: cellHeight*2

                cellHeight: padSize + globalSpacing
                cellWidth: padSize + globalSpacing

                model: padsModel
                delegate: Item {
                    width: GridView.view.cellWidth
                    height: GridView.view.cellHeight
                    Pad {
                        anchors.centerIn: parent
                    }
                }
            }

            Text {
                text: "Knobs"
            }

            GridView {
                id: knobsView

                Layout.fillWidth: true
                Layout.fillHeight: false
                Layout.minimumHeight: cellHeight*2

                cellHeight: knobHeight + globalSpacing
                cellWidth: padSize + globalSpacing

                model: knobsModel
                delegate: Item {
                    width: GridView.view.cellWidth
                    height: GridView.view.cellHeight
                    Knob {
                        anchors.centerIn: parent
                    }
                }
            }

            /*
            Repeater {
                model: padsModel
                delegate: Pad {
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                }
            }
            */
            /*
            Text {
                Layout.columnSpan: 4

                text: "Knobs"
            }

            Repeater {
                model: knobsModel
                delegate: Knob {
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                }
            }
            */
        }
    }
}
