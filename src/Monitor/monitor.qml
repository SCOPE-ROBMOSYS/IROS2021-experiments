import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Layouts 1.2
import QtScxml 5.8

import scope.monitor.MonitorReader 1.0

Window {
    id: root
    width: 640
    height: 480
    minimumWidth: 120
    minimumHeight: 50
    visible: true

    title: "SCOPE Monitor"

    ColumnLayout {
        spacing: 2

        Rectangle {
            Layout.minimumWidth: 100
            Layout.margins: 2
            Layout.preferredWidth: root.width - 2 * Layout.margins
            Layout.preferredHeight: 40

            StateMachineLoader {
                id: loader_01
                source: "qrc:///BatteryLevelMonitor.scxml"
            }

            Connections {
                target: MonitorReader
                onTick: loader_01.stateMachine.submitEvent("tick")
            }

            Connections {
                target: MonitorReader
                onBatteryLevelChanged: loader_01.stateMachine.submitEvent("batteryLevelChanged", { "level": level })
            }

            Connections {
                target: MonitorReader
                onDestinationChangeRequested: loader_01.stateMachine.submitEvent("destinationChangeRequested", { "destination": destination })
            }

            Connections {
                target: MonitorReader
                onDestinationChanged: loader_01.stateMachine.submitEvent("destinationChanged", { "destination": destination })
            }

            RowLayout {
                id: layout_01
                anchors.fill: parent
                spacing: 2

                Rectangle {
                    Layout.fillWidth: true
                    Layout.minimumWidth: 50
                    Layout.preferredWidth: 300
                    Layout.minimumHeight: 30
                    Layout.margins: 2
                    border.color: "black"
                    border.width: 1
                    Text {
                        id: label_01
                        anchors.centerIn: parent
                        text: "[Battery level > 20] (" + MonitorReader.batteryLevel + ")"
                    }
                }

                Rectangle {
                    id: light_01
                    Layout.minimumWidth: 30
                    Layout.preferredWidth: 30
                    Layout.maximumWidth: 30
                    Layout.minimumHeight: width
                    Layout.maximumHeight: width
                    Layout.preferredHeight: width
                    Layout.margins: 2
                    color: "green"
                    border.color: "black"
                    border.width: 1
                    radius: width * 0.5

                    states: [
                        State {
                            name: "Ok"
                            when: loader_01.stateMachine.idle

                            PropertyChanges {
                                target: light_01
                                color: "green"
                            }
                        },
//                         State {
//                             name: "Warning"
//                             when: loader_01.stateMachine.warning
//
//                             PropertyChanges {
//                                 target: light_01
//                                 color: "yellow"
//                             }
//                         },
                        State {
                            name: "Error"
                            when: loader_01.stateMachine.failure

                            PropertyChanges {
                                target: light_01
                                color: "red"
                            }
                        }
                    ]

                }
            } // RowLayout
        } // Rectangle

        Rectangle {
            Layout.minimumWidth: 100
            Layout.margins: 2
            Layout.preferredWidth: root.width - 2 * Layout.margins
            Layout.preferredHeight: 40

            StateMachineLoader {
                id: loader_02
                source: "qrc:///DestinationMonitor.scxml"
            }

            Connections {
                target: MonitorReader
                onTick: loader_02.stateMachine.submitEvent("tick")
            }

            Connections {
                target: MonitorReader
                onBatteryLevelChanged: loader_02.stateMachine.submitEvent("batteryLevelChanged", { "level": level })
            }

            Connections {
                target: MonitorReader
                onDestinationChangeRequested: loader_02.stateMachine.submitEvent("destinationChangeRequested", { "destination": destination })
            }

            Connections {
                target: MonitorReader
                onDestinationChanged: loader_02.stateMachine.submitEvent("destinationChanged", { "destination": destination })
            }


            RowLayout {
                id: layout_02
                anchors.fill: parent
                spacing: 2


                Rectangle {
                    Layout.fillWidth: true
                    Layout.minimumWidth: 50
                    Layout.preferredWidth: 300
                    Layout.minimumHeight: 30
                    Layout.margins: 2
                    border.color: "black"
                    border.width: 1
                    Text {
                        id: label_02
                        anchors.centerIn: parent
                        text: "[Destination] (" + MonitorReader.destination + ")"
                    }
                }

                Rectangle {
                    id: light_02
                    Layout.minimumWidth: 30
                    Layout.preferredWidth: 30
                    Layout.maximumWidth: 30
                    Layout.minimumHeight: width
                    Layout.maximumHeight: width
                    Layout.preferredHeight: width
                    Layout.margins: 2
                    color: "green"
                    border.color: "black"
                    border.width: 1
                    radius: width * 0.5

                    states: [
                        State {
                            name: "Ok"
                            when: loader_02.stateMachine.idleState

                            PropertyChanges {
                                target: light_02
                                color: "green"
                            }
                        },
                        State {
                            name: "Warning"
                            when: loader_02.stateMachine.warning

                            PropertyChanges {
                                target: light_02
                                color: "yellow"
                            }
                        },
                        State {
                            name: "Error"
                            when: loader_02.stateMachine.failure

                            PropertyChanges {
                                target: light_02
                                color: "red"
                            }
                        }
                    ]

                }

            } // RowLayout
        } // Rectangle

    } // ColumnLayout
}
