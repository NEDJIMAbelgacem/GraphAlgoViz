/*
 Copyright (c) 2008-2017, Benoit AUTHEMAN All rights reserved.

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

import QtQuick                   2.9
import QtQuick.Controls          2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Layouts           1.3

import QuickQanava 2.0 as Qan
import "qrc:/QuickQanava" as Qan
import "."
import MyModule             1.0
import "componentCreation.js" as MyScript

Qan.GraphView {
    id: undirected_graphView
    objectName: "undirected_graphView"
    anchors.fill: parent
    //navigable   : true
    resizeHandlerColor: "#03a9f4"       // SAMPLE: Set resize handler color to blue for 'resizable' nodes
    gridThickColor: Material.theme === Material.Dark ? "#4e4e4e" : "#c1c1c1"
    property int edges_counter: 0;
    function create_edge_weight() {
        var obj = Qt.createQmlObject('import QtQuick 2.9; Text {objectName: "weight_item_' + edges_counter + '"; text:"text"; font.pixelSize: 25; color: "red"; x:100; y:100;}',
            undirected_graphView.containerItem, "dynamicSnippet1");
        edges_counter++;
        console.log(obj.objectName);
    }

    onRightClicked: {
        contextMenu.popup();
    }

    Menu {
        id: contextMenu
        MenuItem {
            objectName: "node_creation_menu"
            text: "Create Node"
            onClicked: {
                app_man.createNode();
            }
        }
    }
    Dialog {
        id: weight_dialog
        title: "Select Weight"
        property string weight: "0"
        property var src: undefined
        property var dst: undefined
        property var edge: undefined
        SpinBox {
            id: weight_input
            value : 10
            from: 1; to: 50
        }

        modal: true
        standardButtons: Dialog.Ok
        onAccepted: {
            weight_dialog.weight = weight_input.value;
            console.log("Ok clicked " + weight);
            app_man.createUndirectedEdge(src, dst, edge, weight_dialog.weight);
        }

        onClosed: {
            weight_dialog.weight = weight_input.value;
            console.log("Ok clicked " + weight);
            app_man.createUndirectedEdge(src, dst, edge, weight_dialog.weight);
        }
    }

    graph: CustomGraph {
        id: undirected_graph
        connectorEnabled: true
        objectName: "undirected_graph"
        property var edge_end_shape: Qan.EdgeStyle.None
        property var edge_line_type: Qan.EdgeStyle.Straight
        function update_graph_style() {
            defaultEdgeStyle.dstShape = undirected_graph.edge_end_shape;
            defaultEdgeStyle.srcShape = undirected_graph.edge_end_shape;
            defaultEdgeStyle.lineType = undirected_graph.edge_line_type;
        }
        Component.onCompleted: {
            defaultEdgeStyle.dstShape = edge_end_shape;
            defaultEdgeStyle.srcShape = edge_end_shape;
            defaultEdgeStyle.lineType = edge_line_type;
            console.log("Component.onCompleted");
        }
        onNodeClicked: {
            notifyUser( "Node <b>" + node.label + "</b> clicked" )
            nodeEditor.node = node
        }
        connectorCreateDefaultEdge: false
        onConnectorRequestEdgeCreation: {
          // Do whatever you want here, for example:
            var e = undirected_graph.insertEdge(src, dst);
            //e.setLabel("Label");
            e.item.srcShape = edge_end_shape;
            e.item.dstShape = edge_end_shape;
            weight_dialog.x = e.item.x + undirected_graphView.containerItem.x;
            weight_dialog.y = e.item.y + undirected_graphView.containerItem.y;
            weight_dialog.src = src;
            weight_dialog.dst = dst;
            weight_dialog.edge = e;
            weight_dialog.open();
        }
        onNodeRightClicked: { notifyUser( "Node <b>" + node.label + "</b> right clicked" ) }
        onNodeDoubleClicked: { notifyUser( "Node <b>" + node.label + "</b> double clicked" ) }
    }
    ToolTip { id: toolTip; timeout: 2500 }
    function notifyUser(message) { toolTip.text=message; toolTip.open() }
    Label {
        anchors.left: parent.left; anchors.leftMargin: 15
        anchors.bottom: parent.bottom; anchors.bottomMargin: 15
        text: "Use CTRL+Click to select multiples nodes"
    }

    Frame {
        anchors.left: parent.left; anchors.leftMargin: 15
        anchors.top: parent.top; anchors.topMargin: 15
        ColumnLayout {
            Label {
                objectName: "applied_algorithm"
                text: "Applied algorithm : No algorithm is applied"
                font.pixelSize: 25
            }

            Label {
                objectName: "tsp_path_cost"
                text: "TSP path length : 0"
                font.pixelSize: 25
            }

            Label {
                objectName: "tsp_compute_duration"
                text: "Compute time : 0"
                font.pixelSize: 25
            }
        }
    }

//    Frame {
//        id: scene_controller
//        anchors.bottom: parent.up; anchors.bottomMargin: 15
//        anchors.right: parent.right; anchors.rightMargin: 15
//        ColumnLayout {
//            Label {
//                text: "Hello"
//            }
//            CheckBox {
//                property bool glow_activated: false
//                text: "Glow"
//                onClicked: {
//                    glow_activated = !glow_activated;
//                    if (glow_activated) defaultEdgeStyle.lineType = Qan.EdgeStyle.Curved;
//                    else defaultEdgeStyle.lineType = Qan.EdgeStyle.None;
//                }
//            }
//        }
//    }
    Frame {
        id: nodeEditor
        objectName: "undirected_graph_frame"
        property var node: undefined
        onNodeChanged: nodeItem = node ? node.item : undefined
        property var nodeItem: undefined
        anchors.top: parent.top; anchors.topMargin: 15
        anchors.right: parent.right; anchors.rightMargin: 15
        ColumnLayout {
            Label { text: "Edges end style" }
            ComboBox {
                id: edge_shape_combobox
                property var line_types_array: [Qan.EdgeStyle.None, Qan.EdgeStyle.Arrow, Qan.EdgeStyle.ArrowOpen, Qan.EdgeStyle.Circle, Qan.EdgeStyle.CircleOpen, Qan.EdgeStyle.Rect, Qan.EdgeStyle.RectOpen]
                property var get_selected: (function() {
                    for (var i = 0; i < line_types_array.length; ++i) {
                        if (line_types_array[i] === undirected_graph.edge_end_shape) {
                            return i;
                        }
                    }
                    return 0;
                })
                model: ["None", "Arrow", "ArrowOpen", "Circle", "CircleOpen", "Rect", "RectOpen"]
                enabled: defaultEdgeStyle !== undefined
                currentIndex: get_selected()
                onActivated: {
                    undirected_graph.edge_end_shape = line_types_array[index];
                    defaultEdgeStyle.srcShape = undirected_graph.edge_end_shape;
                    defaultEdgeStyle.dstShape = undirected_graph.edge_end_shape;
                }
            }
            Label { text: "Edges line style" }
            ComboBox {
                model: ["Straight", "Curved", "Orthogonal"]
                enabled: defaultEdgeStyle !== undefined
                currentIndex: defaultEdgeStyle.lineType === Qan.EdgeStyle.Straight ? 0 : 1
                onActivated: {
                    if (index == 0 )        undirected_graph.edge_line_type = Qan.EdgeStyle.Straight
                    else if ( index == 1 )  undirected_graph.edge_line_type = Qan.EdgeStyle.Curved
                    else                    undirected_graph.edge_line_type = Qan.EdgeStyle.Ortho;
                    defaultEdgeStyle.lineType = undirected_graph.edge_line_type;
                }
            }

//            Label { text: nodeEditor.node ? "Editing node <b>" + nodeEditor.node.label + "</b>": "Select a node..." }
            Button {
                text: nodeEditor.node ? "Delete node <b>" + nodeEditor.node.label + "</b>": "Select a node..."
                enabled: nodeEditor.node !== undefined
                onClicked: {
                    app_man.delete_node(nodeEditor.node.label);
                }
            }
        }
    }
//    }
}  // Qan.GraphView

