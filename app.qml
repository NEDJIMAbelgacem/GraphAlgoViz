/*
 Copyright (c) 2008-2018, Benoit AUTHEMAN All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the author or Destrat.io nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL AUTHOR BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

import QtQuick                   2.8
import QtQuick.Controls          2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Layouts           1.3
import QtQuick.Dialogs 1.0

//import QuickQanava          2.0 as Qan
import "qrc:/QuickQanava"   as Qan

ApplicationWindow {
    id: window
    visible: true
    width: 1280; height: 720
    title: "Custom nodes sample"
    Pane { anchors.fill: parent }
    FileDialog {
        id: loadFile
        title: "Load file"
        onAccepted: { app_man.loadGraph(loadFile.fileUrl) }
        onRejected: { }
    }

    FileDialog {
        id: saveFile
        title: "Save file"
        selectExisting: false
        selectFolder: false
        selectMultiple: false
        onAccepted: { app_man.saveGraph(saveFile.fileUrl) }
        onRejected: { }
    }

    Dialog {
        id: weight_dialog
        title: "Select number of nodes"
        property string weight: "0"
        x: window.width / 2 - weight_dialog.width / 2
        y: window.height / 2 - weight_dialog.height / 2
        SpinBox {
            id: nb_nodes_input
            value : 10
            from: 1; to: 50
        }
        modal: true
        standardButtons: Dialog.Ok
        onAccepted: app_man.generate_random_complete_graph(nb_nodes_input.value);

        onClosed: app_man.generate_random_complete_graph(nb_nodes_input.value);
    }

    ColumnLayout {
        anchors.fill: parent
        RowLayout {
            Button {
                text: "Load graph"
                onClicked: { loadFile.open(); }
            }
            Button {
                text: "Save graph"
                onClicked: { saveFile.open(); }
            }
            Button {
                text: "Fix graph layout"
                onClicked: {
                    app_man.fixGraphLayout()
                }
            }
            Button {
                text: "Generate random complete graph"
                onClicked: weight_dialog.open()
            }
            Button {
                text: "Color articulation points"
                onClicked: {
                    app_man.colorizeArticulationPoints()
                }
            }
            Button {
                text: "Held-Karp exact TSP solution"
                onClicked: {
                    app_man.calc_TSP_exactSolution();
                }
            }
            Button {
                text: "Nearest Neighbour TSP solution"
                onClicked: {
                    app_man.calc_TSP_neirest_neighbour_solution();
                }
            }
            Button {
                text: "Permutations exact TSP solution"
                onClicked: {
                    app_man.calc_TSP_exact_solution_permutations();
                }
            }
        }

        TabBar {
            id: tabBar
            objectName: "tab_bar"
            currentIndex: 0
            onCurrentIndexChanged: {
                if (tabBar.currentIndex == 0) {
                    app_man.update_undirected_graph_style();
                } else {
                    app_man.update_directed_graph_style();
                }
            }

            Layout.preferredWidth: 450; Layout.fillHeight: false
            TabButton { text: qsTr("Undirected Graph") }
//            TabButton { text: qsTr("Directed Graph") }
        }
        StackLayout {
            clip: true
            Layout.fillWidth: true; Layout.fillHeight: true
            currentIndex: tabBar.currentIndex
            Item {
                id: undirected_graph_loader
                Loader { anchors.fill: parent; source: "qrc:/undirected_graph.qml"}
            }
            Item {
                id: directed_graph_loader
                Loader { anchors.fill: parent; source: "qrc:/directed_graph.qml"}
            }
        }
    }
    RowLayout {
        anchors.top: parent.top;    anchors.right: parent.right
        CheckBox {
            text: qsTr("Dark")
            checked: ApplicationWindow.contentItem.Material.theme === Material.Dark
            onClicked: ApplicationWindow.contentItem.Material.theme = checked ? Material.Dark : Material.Light
        }
    }
}

