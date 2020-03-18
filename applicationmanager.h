#ifndef APPLICATIONMANAGER_H
#define APPLICATIONMANAGER_H
#include <QObject>

#include <QGuiApplication>
#include <QtQml>
#include <QQuickStyle>
#include <QVariant>
#include <QtQuick>
#include <QtQuick/QQuickItem>
#include "QFrame"
#include <QtMath>

#include <random>
#include <chrono>

// QuickQanava headers
#include <QuickQanava.h>
#include <QDebug>

#include "graph.h"
#include "customnode.h"
#include "customgraph.h"

struct GraphColorsConfig {
    QMap<QString, QColor> config;
};


class WeightItemConfig : public QObject {
    Q_OBJECT
public:
    qan::Edge* edge = nullptr;
    QQuickItem* item = nullptr;
    QQuickItem* container = nullptr;
    qan::GraphView* graph_view = nullptr;
    ~WeightItemConfig() {
        delete item;
    }
public slots:
    void UpdatePosition() {
        if (item == nullptr || edge == nullptr) return;
        QQuickItem* src_item = edge->getSource()->getItem();
        QQuickItem* dst_item = edge->getDestination()->getItem();
        QPointF p1 = edge->getItem()->mapToItem(container, edge->getItem()->getP1());//src_item->position();
        QPointF p2 = edge->getItem()->mapToItem(container, edge->getItem()->getP2());//dst_item->position();
        QPointF p = (p1 + p2) / 2.0f;
        item->setPosition(p);
    }
};

class ApplicationManager : public QObject {
    Q_OBJECT
private:
    QQmlApplicationEngine& engine;
    QQuickItem* tab_bar = nullptr;
    // undirected_graph stuff
    CustomGraph* undirected_qangraph;
    qan::GraphView* undirected_graphView;
    Graph* undirected_graph;
    QMap<QString, qan::Node*> undirected_scene_nodes;
    QMap<QString, qan::NodeItem*> undirected_node_item;
    QVector<qan::Edge*> undirected_edges;
    QVector<WeightItemConfig*> edge_weight_config;
    QMap<qan::Edge*, int> edge_weight;
    QVector<qan::Node*> undirected_nodes;
    qreal graphViewX, graphViewY, graphViewScale;
    QMap<QPair<QString, QString>, qan::Edge*> edges_map;
    QQuickItem* node_creation_menu_item = nullptr;

    QColor articulation_point_color { 255, 0, 0 };
    QColor normal_node_color { 255, 255, 255 };

    QQuickItem* applied_algorithm_label = nullptr;
    QQuickItem* tsp_solution_cost_label = nullptr;
    QQuickItem* tsp_duration_label = nullptr;

private:
    int tab_index() {
        // 0 for undirected & 1 for directed
        return tab_bar->property("currentIndex").toInt();
    }

    void set_tsp_solution_cost(int cost, int time) {
        tsp_solution_cost_label->setProperty("text", QString("TSP problem solution %1").arg(cost));
        tsp_duration_label->setProperty("text", QString("TSP problem time %1 microseconds").arg(time));
    }
    WeightItemConfig* create_edge_weight_item(QString weight) {
        //graph_view->children().push_back(qobject_cast<QObject* const>(component));
        static int edges_counter = 0;
        QQuickItem* item = nullptr;
        bool succ = QMetaObject::invokeMethod((QObject*)undirected_graphView, "create_edge_weight");
        QString item_objectName = QString("weight_item_%1").arg(edges_counter);
        item = undirected_graphView->findChild<QQuickItem*>(item_objectName);
        ++edges_counter;
        item->setProperty("text", weight);
        WeightItemConfig* conf = new WeightItemConfig;
        conf->item = item;
        conf->edge = undirected_edges.back();
        conf->container = undirected_graphView->getContainerItem();
        conf->graph_view = undirected_graphView;
        edge_weight_config.append(conf);
        return conf;
    }
public:
    ApplicationManager(QQmlApplicationEngine& app_engine, QObject* parent = nullptr);
    void FillQanGraph(qan::Graph* qan_graph, Graph* g) {
        for (auto* ptr : edge_weight_config) delete ptr;
        edge_weight_config.clear();
        update_undirected_graph_style();
        if (tab_index() == 0) {
            undirected_scene_nodes.clear();
            QMap<QString, QSet<QString>>& adj_list = g->GetAdjacencyList();
            for (QString s : adj_list.keys()) {
                undirected_scene_nodes[s] = undirected_qangraph->insertCustomNode();
                undirected_scene_nodes[s]->setLabel(s);
                undirected_node_item[s] = undirected_scene_nodes[s]->getItem();
                undirected_node_item[s]->setPosition(undirected_graph->getNodePosition(s));
            }
            QMap<QString, QSet<QString>> adj_list2;
            for (QString s : adj_list.keys()) {
                for (QString s2 : adj_list[s]) {
                    if (undirected_graph->getType() == Graph::Undirected && adj_list2[s2].contains(s)) continue;
                    if (undirected_graph->getType() == Graph::Weighted && adj_list2[s2].contains(s)) continue;
                    qan::Edge* edge = qan_graph->insertEdge(undirected_scene_nodes[s], undirected_scene_nodes[s2]);
                    qan::Node* src = undirected_scene_nodes[s];
                    qan::Node* dest = undirected_scene_nodes[s2];
                    edges_map[qMakePair(src->getLabel(), dest->getLabel())] = edge;
                    edges_map[qMakePair(dest->getLabel(), src->getLabel())] = edge;
                    undirected_edges.push_back(edge);
                    WeightItemConfig* c = create_edge_weight_item(QString("%1").arg(undirected_graph->getEdgeWeight(s, s2)));
                    c->UpdatePosition();
                    QObject::connect(c->edge->getItem(), SIGNAL(labelPosChanged()), c, SLOT(UpdatePosition()));

                    adj_list2[s].insert(s2);

                }
            }

        }
    }
public slots:
    void graphView_container_changed() {
        QQuickItem* container = undirected_graphView->getContainerItem();
        graphViewX = container->x();
        graphViewY = container->y();
        graphViewScale = container->scale();
        for (auto* conf : edge_weight_config) conf->UpdatePosition();
    }
    void loadGraph(QString file) {
        clear();
        Graph* graph = new Graph(file);
        if (graph->getType() == Graph::Undirected) {
            undirected_qangraph->clear();
            if (undirected_graph != nullptr) delete undirected_graph;
            undirected_graph = graph;
            FillQanGraph(undirected_qangraph, undirected_graph);
        }
        if (graph->getType() == Graph::Weighted) {
            undirected_qangraph->clear();
            if (undirected_graph != nullptr) delete undirected_graph;
            undirected_graph = graph;
            FillQanGraph(undirected_qangraph, undirected_graph);
        }
//        for (qan::Edge* e : undirected_edges) {
//            e->style()->setSrcShape(qan::EdgeStyle::ArrowShape::None);
//            e->style()->setDstShape(qan::EdgeStyle::ArrowShape::None);
//        }
    }

    void saveGraph(QString file_name) {
        QFile file(QUrl(file_name).toLocalFile());
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << "Error writing to file " << file_name;
            return;
        }
        QTextStream out(&file);

        if (tab_index() == 0) {
            QMap<QPair<QString, QString>, int>& edges_list = undirected_graph->GetEdgesList();
            auto& adj_list = undirected_graph->GetAdjacencyList();
            out << "WEIGHTED" << "\n";
            out << "NODES" << "\n";
            out << adj_list.size() << "\n";
            for (QString n1 : adj_list.keys()) {
                out << n1 << " POSITION " << undirected_node_item[n1]->position().x();
                out << " " << undirected_node_item[n1]->position().y() << "\n";
            }
            out << "EDGES_LIST" << "\n";
            out << edges_list.size() << "\n";
            for (auto p : edges_list.keys()) {
                out << p.first << " " << p.second << " " << edges_list[p] << "\n";
            }
        }
    }

    void colorizeArticulationPoints() {
        if (tab_index() == 0) {
            for (QString s : undirected_node_item.keys()) {
                undirected_node_item[s]->setProperty("node_color", normal_node_color);
            }
            for (QString art_p : undirected_graph->getArticulationPoints()) {
                undirected_node_item[art_p]->setProperty("node_color", articulation_point_color);
            }
            undirected_graph->print_graph();
        }
    }

    void createNode() {
        int label_int = this->undirected_graph->getNbNodes();
        QString label = QString("%1").arg(label_int + 1);
        QPointF mp = QCursor::pos();
        createNode(mp.x(), mp.y(), label);
    }

    void createNode(double x, double y, QString label) {
        if (tab_index() == 0) {
            undirected_graph->AddNode(label);
            undirected_scene_nodes[label] = undirected_qangraph->insertCustomNode();
            undirected_scene_nodes[label]->setLabel(label);
            undirected_node_item[label] = undirected_scene_nodes[label]->getItem();
            QPointF p = undirected_graphView->getContainerItem()->mapFromGlobal(QPoint(x, y));
            p.setX(p.x() - undirected_node_item[label]->width() / 2.0);
            p.setY(p.y() - undirected_node_item[label]->height() / 2.0);
            undirected_node_item[label]->setPosition(p);
            undirected_node_item[label]->setProperty("node_color", normal_node_color);
        }
    }

    void createUndirectedEdge(qan::Node* src, qan::Node* dest, qan::Edge* edge, QString edge_weight) {
        undirected_graph->AddWeightedEdge(src->getLabel(), dest->getLabel(), edge_weight.toInt());
        qDebug() << "creating undirected edge from " << src->getLabel() << " To " << dest->getLabel();
        edges_map[qMakePair(src->getLabel(), dest->getLabel())] = edge;
        edges_map[qMakePair(dest->getLabel(), src->getLabel())] = edge;
        undirected_edges.push_back(edge);
        WeightItemConfig* c = create_edge_weight_item(edge_weight);
        c->UpdatePosition();
        QObject::connect(c->edge->getItem(), SIGNAL(labelPosChanged()), c, SLOT(UpdatePosition()));
    }

    void fixGraphLayout() {
        if (tab_index() == 0) {
            QMap<QString, QPointF> node_pos = undirected_graph->getGraphNodesLayout();//graph->getRandomNodesPositions();
            for (QString n : node_pos.keys()) {
                undirected_node_item[n]->setPosition(node_pos[n]);
            }
        }
    }

    void update_undirected_graph_style() {
        QMetaObject::invokeMethod((QObject*)undirected_qangraph, "update_graph_style");
    }

    void generate_random_complete_graph(QVariant nb_nodes, int rand_start=10, int rand_end=30) {
        clear();
        qreal circle_radius = 300.0f;
        qreal circle_x = undirected_graphView->width() / 2.0f, circle_y = undirected_graphView->height() / 2.0f;
        int n = nb_nodes.toInt();
        for (int i = 1; i <= n; ++i) {
            qreal rads = 2.0 * (i - 1) / n * M_PI;
            float x = circle_x + circle_radius * qSin(rads);
            float y = circle_y + circle_radius * qCos(rads);
            QPointF p(x, y);
            p = undirected_graphView->getContainerItem()->mapToGlobal(p);
            createNode(p.x(), p.y(), QString("%1").arg(i));
        }

        for (int i = 1; i <= n; ++i) {
            qan::Node* src = undirected_scene_nodes[QString("%1").arg(i)];
            for (int j = i + 1; j <= n; ++j) {
                qan::Node* dst = undirected_scene_nodes[QString("%1").arg(j)];
                undirected_edges.push_back(undirected_qangraph->insertCustomEdge(src, dst));
                createUndirectedEdge(src, dst, undirected_edges.back(), QString("%1").arg(rand_start + rand() % (rand_end - rand_start)));
            }
        }
        qDebug() << __FUNCTION__;
    }

    void calc_TSP_exactSolution() {
        using milli = std::chrono::duration<int, std::micro>;

        auto start = std::chrono::system_clock::now();
        auto p = undirected_graph->get_Held_Karp_solution();
        auto end = std::chrono::system_clock::now();
        milli elapsed_milliseconds = std::chrono::duration_cast<milli>(end - start);

        for (qan::Edge* e : undirected_edges) e->getItem()->setProperty("color", "black");
        for (QPair<QString, QString> v : p.first) {
            qDebug() << v.first << " " << v.second;
            qan::Edge* e = edges_map[v];
            e->getItem()->setProperty("color", "blue");
        }
        set_tsp_solution_cost(p.second, elapsed_milliseconds.count());

        applied_algorithm_label->setProperty("text", "Applied algorithm : Held-Karp exact solution (n^2)*2^n");
    }

    void calc_TSP_neirest_neighbour_solution() {
        using milli = std::chrono::duration<int, std::micro>;

        auto start = std::chrono::system_clock::now();
        auto p = undirected_graph->get_nearest_neighbour_solution();
        auto end = std::chrono::system_clock::now();
        milli elapsed_milliseconds = std::chrono::duration_cast<milli>(end - start);

        for (qan::Edge* e : undirected_edges) e->getItem()->setProperty("color", "black");
        for (QPair<QString, QString> v : p.first) {
            qDebug() << v.first << " " << v.second;
            qan::Edge* e = edges_map[v];
            e->getItem()->setProperty("color", "blue");
        }
        set_tsp_solution_cost(p.second, elapsed_milliseconds.count());

        applied_algorithm_label->setProperty("text", "Applied algorithm : Nearest Neighbour approximation");
    }

    void calc_TSP_exact_solution_permutations() {
        using milli = std::chrono::duration<int, std::micro>;

        auto start = std::chrono::system_clock::now();
        auto p = undirected_graph->get_factorial_method_tsp_solution();
        auto end = std::chrono::system_clock::now();
        milli elapsed_microseconds = std::chrono::duration_cast<milli>(end - start);

        for (qan::Edge* e : undirected_edges) e->getItem()->setProperty("color", "black");
        for (QPair<QString, QString> v : p.first) {
            qDebug() << v.first << " " << v.second;
            qan::Edge* e = edges_map[v];
            e->getItem()->setProperty("color", "blue");
        }
        set_tsp_solution_cost(p.second, elapsed_microseconds.count());
        applied_algorithm_label->setProperty("text", "Applied algorithm : exact solution algorithm (n! complexity)");
    }

    void clear() {
        undirected_qangraph->clear();
        undirected_graph->clear();
        undirected_scene_nodes.clear();
        undirected_node_item.clear();
        undirected_edges.clear();
        for (auto* c : edge_weight_config) delete c;
        edge_weight_config.clear();
        edge_weight.clear();
        undirected_nodes.clear();
        edges_map.clear();
    }
};

#endif // APPLICATIONMANAGER_H
