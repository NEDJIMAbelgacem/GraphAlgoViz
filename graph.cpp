#include "graph.h"
#include <QDebug>
Graph::Graph(QString file_url)
{

    QFile file(QUrl(file_url).toLocalFile());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "can't open file";
        qDebug() << file.errorString();
        exit(-1);
    }
    // graph type
    QString graph_type = file.readLine().trimmed();
    if (graph_type == "DIRECTED") {
        this->type = Graph::GraphType::Directed;
    } else if (graph_type == "UNDIRECTED") {
        this->type = Graph::GraphType::Undirected;
    } else if (graph_type == "WEIGHTED") {
        this->type = Graph::GraphType::Weighted;
    }else {
        qDebug() << "UNKNOWN GRAPH TYPE !!!";
        exit(-1);
    }
    if (this->type == Graph::GraphType::Weighted) {
        QString line = file.readLine().trimmed();
        if (line != "NODES") {
            qDebug() << "Error loading weighted graph";
            exit(-1);
        }
        int nb_nodes = file.readLine().trimmed().toInt();
        for (int i = 0; i < nb_nodes; ++i) {
            QString nodes_line = file.readLine().trimmed();
            QStringList nodes = nodes_line.split(" ", QString::SplitBehavior::SkipEmptyParts);
            qDebug() << nodes;
            QString node_name = nodes[0];
            int neighbours_start = 1;
            if (nodes.size() > 1 && nodes[1] == "POSITION") {
                neighbours_start = 4;
                QString x_str = nodes[2], y_str = nodes[3];
                node_position[nodes[0]] = QPoint(x_str.toDouble(), y_str.toDouble());
            }
            /*adj_list[node_name] = QSet<QString>();
            for (int j = neighbours_start; j < nodes.size(); ++j) {
                adj_list[node_name].insert(nodes[j]);
            }*/
        }
        line = file.readLine().trimmed();
        if (line != "EDGES_LIST") {
            qDebug() << "Error loading weighted graph";
            exit(-1);
        }
        int nb_edges = file.readLine().trimmed().toInt();
        for (int i = 0; i < nb_edges; ++i) {
            QString edge_line = file.readLine().trimmed();
            QStringList edge = edge_line.split(" ", QString::SplitBehavior::SkipEmptyParts);
            QString n1 = edge[0];
            QString n2 = edge[1];
            int weight = edge[2].toInt();
            adj_list[n1].insert(n2);
            weights_map[qMakePair(n1, n2)] = weight;
        }
        return;
    }
    QString spec_type = file.readLine().trimmed();
    if (spec_type == "ADJ_LIST") {
        int nb_nodes = file.readLine().trimmed().toInt();
        for (int i = 0; i < nb_nodes; ++i) {
            QString nodes_line = file.readLine().trimmed();
            QStringList nodes = nodes_line.split(" ", QString::SplitBehavior::SkipEmptyParts);
            qDebug() << nodes;
            QString node_name = nodes[0];
            int neighbours_start = 1;
            if (nodes.size() > 1 && nodes[1] == "POSITION") {
                neighbours_start = 4;
                QString x_str = nodes[2], y_str = nodes[3];
                node_position[nodes[0]] = QPoint(x_str.toDouble(), y_str.toDouble());
            }
            adj_list[node_name] = QSet<QString>();
            for (int j = neighbours_start; j < nodes.size(); ++j) {
                adj_list[node_name].insert(nodes[j]);
            }
        }
    } else {
        qDebug() << "Unknown specification type : " << spec_type;
        exit(-1);
    }
    qDebug() << IsValid();
    print_graph();
}

Graph::Graph() {

}
