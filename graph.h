#ifndef GRAPH_H
#define GRAPH_H

#include <QMap>
#include <QString>

#include <QFile>
#include <QUrl>
#include <QDebug>
#include <QPoint>
#include <QSet>
#include <QtMath>
#include <iostream>
#include <limits>

#include <algorithm>

template<typename T>
bool operator<(QSet<T> s1, QSet<T> s2) {
    auto it1 = s1.begin();
    auto it2 = s2.begin();
    while (it1 != s1.end() && it2 != s2.end()) {
        if (*it1 < *it2) return true;
        if (*it1 > *it2) return false;
        ++it1;
        ++it2;
    }
    if (it1 == s1.end()) return true;
    return false;
}

class Graph
{
public:
    enum GraphType {
        Directed,
        Undirected,
        Weighted
    };
private:
    QMap<QString, QSet<QString>> adj_list;
    QMap<QPair<QString, QString>, int> weights_map;
    Graph::GraphType type;
    QMap<QString, QPoint> node_position;
public:
    Graph(QString file_url);
    Graph();

    void print_graph() {
        std::cout << "Graph type : ";
        if (type == Directed) std::cout << "Directed" << std::endl;
        if (type == Undirected) std::cout << "Unirected" << std::endl;
        if (type == Weighted) std::cout << "Weighted" << std::endl;
        for (QString n1 : adj_list.keys()) {
            std::cout << n1.toStdString() << " -> ";
            for (QString n2 : adj_list[n1]) std::cout << n2.toStdString() << " ";
            std::cout << std::endl;
        }
    }

    bool IsValid() {
        for (auto& p : adj_list) {
            for (QString s : p) {
                if (!adj_list.contains(s)) return false;
            }
        }
        return true;
    }

    Graph::GraphType getType() { return type; }
    void setType(Graph::GraphType t) { this->type = t; }

    QPoint getNodePosition(QString node) { return node_position[node]; }
    void setNodePosition(QString node, QPoint p) { node_position[node] = p; }

    int getNbNodes() {
        return adj_list.size();
    }

    void AddNode(QString node_id) {
        adj_list[node_id] = QSet<QString>();
    }

    void AddDirectedEdge(QString src, QString dest) {
        adj_list[src].insert(dest);
    }

    void AddUndirectedEdge(QString src, QString dest) {
        adj_list[src].insert(dest);
        adj_list[dest].insert(src);
    }

    void AddWeightedEdge(QString src, QString dest, int weight) {
        adj_list[src].insert(dest);
        adj_list[dest].insert(src);
        weights_map[qMakePair(src, dest)] = weight;
        weights_map[qMakePair(dest, src)] = weight;
    }

    int getEdgeWeight(QString n1, QString n2) {
        QPair<QString, QString> p = {n1, n2};
        if (!weights_map.contains(p)) return 999999;
        return weights_map[p];
    }

    void printGraph() {
        for (QString n1 : adj_list.keys()) {
            qDebug() << n1 << " : ";
            for (QString n2 : adj_list[n1]) qDebug() << n2 << " ";
        }
    }

    QMap<QString, QSet<QString>>& GetAdjacencyList() {
        return adj_list;
    }

    QMap<QPair<QString, QString>, int>& GetEdgesList() {
        return weights_map;
    }

    // naive algorithm
    // The idea is to take every node that have more than 2 neighbours and delete that node (I just mark it as visited from the start)
    // then do DFS from one neighbour node to the other nodes, if all the parent node neighbours can be visited then the node is not
    // an articulation point
    QSet<QString> getArticulationPoints_inefficient() {
        QSet<QString> articulation_points;
        QSet<QString> visited;
        auto bfs_search = [&](QString curr, QSet<QString>& dest, auto& func) -> bool {
            if (dest.contains(curr)) dest.remove(curr);
            if (dest.isEmpty()) return true;
            visited.insert(curr);
            for (QString next : adj_list[curr]) {
                if (visited.contains(next)) continue;
                if (func(next, dest, func)) return true;
            }
            return false;
        };
        for (QString point : adj_list.keys()) {
            if (adj_list[point].size() > 1) {
                bool is_articulation_point = false;
                for (QString src : adj_list[point]) {
                    QSet<QString> dest = adj_list[point];
                    visited.clear();
                    visited.insert(point);
                    if (!bfs_search(src, dest, bfs_search)) {
                        is_articulation_point = true;
                        break;
                    }
                }
                if (is_articulation_point) articulation_points.insert(point);
            }
        }
        return articulation_points;
    }

    // Efficient algorithm
    // The algorithm is based on Tarjan strongly connected components algorithm
    QSet<QString> getArticulationPoints() {
        printGraph();
        QSet<QString> articulation_points;
        QSet<QString> visited;
        QMap<QString, int> visit_time;
        QMap<QString, int> low_node;
        QMap<QString, QString> parent;
        int time = 0;
        auto bfs_search = [&](QString curr, auto& func) -> void {
            visit_time[curr] = time++;
            visited.insert(curr);
            low_node[curr] = visit_time[curr];
            int nb_children = 0;
            for (QString next : adj_list[curr]) {
                if (!visited.contains(next)) {
                    nb_children++;
                    parent[next] = curr;
                    func(next, func);
                    // take the smallest reachable node
                    if (low_node[curr] > low_node[next]) {
                        low_node[curr] = low_node[next];
                    }
                    // if it is root node and has more than 1 child
                    if (parent.find(curr) == parent.end() && nb_children > 1) {
                        articulation_points.insert(curr);
                    }
                    // if it is an internal node and
                    if (parent.find(curr) != parent.end() && low_node[next] >= visit_time[curr]) {
                        articulation_points.insert(curr);
                    }
                } else {
                    // take the smallest neighbour visit time
                    if (visit_time[next] < low_node[curr]) {
                        low_node[curr] = visit_time[next];
                    }
                }
            }
        };
        for (QString point : adj_list.keys()) {
            if (adj_list[point].size() > 1 && !visited.contains(point)) {
                bfs_search(point, bfs_search);
            }
        }
        for (QString point : adj_list.keys()) {
            qDebug() << "node " << point << " : " << "visit_time=" << visit_time[point] << ", low_node=" << low_node[point];
        }
        return articulation_points;
    }

    QMap<QString, QPointF> getRandomNodesPositions(double width = 1280.0, double height = 720.0) {
        QMap<QString, QPointF> res;
        for (QString n : adj_list.keys()) {
            res[n] = QPointF(rand() % (int)width, rand() % (int)height);
        }
        return res;
    }

    QMap<QString, QPointF> node_pos;

    // Fruchterman & Reingold graph layout algorithm
    QMap<QString, QPointF> getGraphNodesLayout(double width = 1280.0, double height = 720.0) {
        const int NB_ITERATIONS = 100;
        const QPointF displacement_temp(100.0, 100.0);
        static bool executed = false;
        if (!executed) {
            node_pos = getRandomNodesPositions(width, height);
            executed = true;
        }
        QMap<QString, QPointF> node_disp;
        double area = width * height;
        double k = qSqrt(area / node_pos.size());
        auto fa = [&](double x) { return x * x / k; };
        auto fr = [&](double x) { return k * k / x; };
        auto length = [&](QPointF p) {
            return qSqrt(p.x() * p.x() + p.y() * p.y());
        };
        for (int i = 0; i < NB_ITERATIONS; ++i) {
            // calculate repulsive forces
            for (QString v : node_pos.keys()) {
                node_disp[v] = QPointF(0.0, 0.0);
                for (QString u : node_pos.keys()) {
                    if (u == v) continue;
                    QPointF delta = node_pos[v] - node_pos[u];
                    double abs_delta = length(delta);
                    node_disp[v] += delta / abs_delta * fr(abs_delta);
                }
            }
            // calculate attractive forces
            for (QString u : adj_list.keys()) {
                for (QString v : adj_list[u]) {
                    QPointF delta = node_pos[v] - node_pos[u];
                    double abs_delta = length(delta);
                    node_disp[v] -= delta / abs_delta * fa(abs_delta) / (1.0 + adj_list[u].size() / 2);
                    node_disp[u] += delta / abs_delta * fa(abs_delta) / (1.0 + adj_list[v].size() / 2);
                }
            }
            // limit max displacement to temperature t and prevent from displacement outside frame
            for (QString v : adj_list.keys()) {
                double abs_disp = length(node_disp[v]);
                QPointF p = node_disp[v];//qMin(displacement_temp, node_disp[v]);
                if (abs_disp > length(displacement_temp)) p = displacement_temp;
                node_pos[v].setX(node_pos[v].x() + node_disp[v].x() / abs_disp * p.x());
                node_pos[v].setY(node_pos[v].y() + node_disp[v].y() / abs_disp * p.y());
            }
        }

        double center_x = 0.0, center_y = 0.0;
        for (QPointF p : node_pos.values()) {
            center_x += p.x();
            center_y += p.y();
        }
        center_x /= (double)node_pos.values().size();
        center_y /= (double)node_pos.values().size();

        for (QString n : node_pos.keys()) {
            node_pos[n].setX(node_pos[n].x() - center_x);
            node_pos[n].setY(node_pos[n].y() - center_y);
        }

        return node_pos;
    }

    // Kusaraju Strongly connected components algorithm
    // I used Kusarju because I wanted to practice it not because it is better than Tarjan's algorithm
    QVector<QVector<QString>> getStronglyConnectedComponents() {
        QMap<QString, QSet<QString>> transpose_adj_list;
        for (QString n1 : adj_list.keys()) {
            for (QString n2 : adj_list[n1]) {
                transpose_adj_list[n2].insert(n1);
            }
        }
        auto dfs = [&](QString node, QMap<QString, QSet<QString>>& adj_list, QSet<QString>& visited, auto& dfs_func) -> QVector<QString> {
            if (visited.contains(node)) return QVector<QString>();
            QVector<QString> res;
            visited.insert(node);
            for (QString next : adj_list[node]) {
                for (QString n : dfs_func(next, adj_list, visited, dfs_func)) res.push_back(n);
            }
            res.push_back(node);
            return res;
        };
        QSet<QString> visited;
        QVector<QString> stack = dfs(*adj_list.keyBegin(), adj_list, visited, dfs);
//        qDebug() << stack;
        visited.clear();
        QMap<QString, int> component_number;
        int curr_component = 0;
        for (auto it = stack.rbegin(); it != stack.rend(); ++it) {
            if (visited.contains(*it) || component_number.contains(*it)) continue;
            for (QString node : dfs(*it, transpose_adj_list, visited, dfs)) component_number[node] = curr_component;
            curr_component++;
        }
        QVector<QVector<QString>> components(curr_component);
        for (QString node : component_number.keys()) components[component_number[node]].push_back(node);
        qDebug() << "Strongly connected components : ";
        for (QVector<QString> v : components) qDebug() << v;
        return components;
    }

    QVector<QString> getStrongArticulationPoints() {
        QVector<QVector<QString>> components = getStronglyConnectedComponents();
        QMap<QString, QSet<QString>> adj_list_backup = this->adj_list;
        QVector<QString> strong_articulation_points;
        for (QVector<QString> comp : components) {
            if (comp.size() == 1) continue;
            this->adj_list.clear();
            for (QString n1 : adj_list_backup.keys()) {
                if (!comp.contains(n1)) continue;
                for (QString n2 : adj_list_backup[n1]) {
                    if (!comp.contains(n2)) continue;
                    this->adj_list[n1].insert(n2);
                    this->adj_list[n2];
                }
            }
            for (QString art : getArticulationPoints()) {
                strong_articulation_points.push_back(art);
            }
        }
        this->adj_list = adj_list_backup;
        qDebug() << "strong articulation points : ";
        qDebug() << strong_articulation_points;
        return strong_articulation_points;
    }

    QPair<QVector<QPair<QString, QString>>, int> get_Held_Karp_solution() {
        if (getNbNodes() <= 2) return qMakePair(QVector<QPair<QString, QString>>(), -1);
        QMap<QString, int> nodes_mapping;
        QMap<int, QString> nodes_mapping_inv;
        QSet<int> nodes;
        int i = 1;
        for (QString n : this->adj_list.keys()) {
            nodes_mapping[n] = i;
            nodes_mapping_inv[i] = n;
            nodes.insert(i);
            ++i;
        }
        QMap<int, QMap<int, int>> w_matrix;
        for (auto p : weights_map.keys()) {
            w_matrix[nodes_mapping[p.first]][nodes_mapping[p.second]] = weights_map[p];
        }
        auto distance_between = [&](int i, int j) -> int {
            if (!w_matrix.contains(i) || !w_matrix[i].contains(j)) return 999999;
            return w_matrix[i][j];
        };
        auto conv = [&](QSet<int> st) -> QString {
            QString res;
            for (int i : st) res += QString("_%1").arg(i);
            return res;
        };
        QMap<QString, QMap<int, int>> solutions;
        QMap<QString, QMap<int, int>> solution_parent;
        int n = getNbNodes();
        int depth = 0;
        auto find_solution = [&](QSet<int> s, int i, auto& f) -> int {
            qDebug() << QString("%1 -> ").arg(depth) << " " << s << " " << i;
            QString str = conv(s);
            if (solutions.contains(str) && solutions[str].contains(i)) return solutions[str][i];
            if (s.size() == 2) {
                solutions[str][i] = distance_between(1, i);
                solution_parent[str][i] = 1;
                return solutions[str][i];
            }
            s.remove(i);
            int min = 9999;
            int min_node = -1;
            for (int j : s) {
                if (j == i || j == 1) continue;
                depth++;
                int d = f(s, j, f) + distance_between(i, j);
                depth--;
                if (min > d) {
                    min = d;
                    min_node = j;
                }
            }
            s.insert(i);
            solutions[str][i] = min;
            solution_parent[str][i] = min_node;
            return min;
        };

        int best_start = 2;
        int circuit_length = 99999;

        for (int i = 2; i <= n; ++i) {
            int d = find_solution(nodes, i, find_solution) + distance_between(1, i);
            if (circuit_length > d) {
                circuit_length = d;
                best_start = i;
            }
        }
        qDebug() << "Circuit length : " << circuit_length;
        if (circuit_length >= 999999) return qMakePair(QVector<QPair<QString, QString>>(), -1);
        int k = best_start;
        QVector<QString> circuit;
        while (k != 1) {
            circuit.push_back(nodes_mapping_inv[k]);
            QString str = conv(nodes);
            nodes.remove(k);
            k = solution_parent[str][k];
        }
        circuit.push_back(nodes_mapping_inv[k]);
        qDebug() << circuit;
        QVector<QPair<QString, QString>> res;
        for (int i = 0; i < circuit.size(); ++i) {
            res.push_back(qMakePair(circuit[i], circuit[(i + 1) % circuit.size()]));
        }
        return qMakePair(res, circuit_length);
    }

    QPair<QVector<QPair<QString, QString>>, int> get_nearest_neighbour_solution() {
        if (getNbNodes() <= 2) return qMakePair(QVector<QPair<QString, QString>>(), -1);
        QVector<QString> nodes;
        int total_cost = 0;
        int min_edge_weight = 999999;
        QPair<QString, QString> min_edge;
        for (QPair<QString, QString> e : weights_map.keys()) {
            if (min_edge_weight > weights_map[e]) {
                min_edge_weight = weights_map[e];
                min_edge = e;
            }
        }
        nodes.push_back(min_edge.first);
        nodes.push_back(min_edge.second);
        total_cost += min_edge_weight;
        while (nodes.size() != getNbNodes()) {
            qDebug() << nodes;
            QVector<QString> v = {nodes.front(), nodes.back()};
            min_edge_weight = 999999;
            for (QString n1 : v) {
                for (QString n2 : adj_list[n1]) {
                    if (nodes.contains(n2)) continue;
                    QPair<QString, QString> e = {n1, n2};
                    if (min_edge_weight > weights_map[e]) {
                        min_edge_weight = weights_map[e];
                        min_edge = e;
                    }
                }
            }
            if (min_edge_weight == 999999) return qMakePair(QVector<QPair<QString, QString>>(), -1);
            QString n1 = min_edge.first;
            QString n2 = min_edge.second;
            if (nodes.contains(n2)) qSwap(n1, n2);
            if (n1 == nodes[0]) nodes.push_front(n2);
            else nodes.push_back(n2);
            total_cost += min_edge_weight;
        }
        int w = getEdgeWeight(nodes.front(), nodes.back());
        if (w == 999999) return qMakePair(QVector<QPair<QString, QString>>(), -1);
        total_cost += w;
        QVector<QPair<QString, QString>> res;
        for (int i = 0; i < nodes.size(); ++i) res.push_back(qMakePair(nodes[i], nodes[(i + 1) % nodes.size()]));
        return qMakePair(res, total_cost);
    }

    QPair<QVector<QPair<QString, QString>>, int> get_factorial_method_tsp_solution() {
        if (getNbNodes() <= 2) return qMakePair(QVector<QPair<QString, QString>>(), -1);
        int min_cost = 999999;
        QVector<QString> best_cycle;
        QVector<QString> nodes = adj_list.keys().toVector();
        std::sort(nodes.begin(), nodes.end());
        do {
            int cost = 0;
            bool invalid_cycle = false;
            for (int i = 0; i < nodes.size(); ++i) {
                auto edge = qMakePair(nodes[i], nodes[(i + 1) % nodes.size()]);
                if (!weights_map.contains(edge)) {
                    invalid_cycle = true;
                    break;
                }
                cost += weights_map[edge];
            }
            if (invalid_cycle) continue;
            if (min_cost > cost) {
                min_cost = cost;
                best_cycle = nodes;
            }
        } while (std::next_permutation(nodes.begin(), nodes.end()));
        if (best_cycle.size() == 0) return qMakePair(QVector<QPair<QString, QString>>(), -1);
        QVector<QPair<QString, QString>> res;
        for (int i = 0; i < best_cycle.size(); ++i) res.push_back(qMakePair(best_cycle[i], best_cycle[(i + 1) % best_cycle.size()]));
        return qMakePair(res, min_cost);
    }

    void clear() {
        adj_list.clear();
        weights_map.clear();
        node_position.clear();
    }
};

#endif // GRAPH_H
