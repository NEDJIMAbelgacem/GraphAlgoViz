#include "applicationmanager.h"

ApplicationManager::ApplicationManager(QQmlApplicationEngine& app_engine, QObject* parent)
    : QObject(parent), engine(app_engine)
{
    app_engine.rootContext()->setContextProperty("app_man", this);
    app_engine.load(QUrl("qrc:/app.qml"));
    for (const auto obj : app_engine.rootObjects()) {
        for (QQuickItem* item :  obj->findChildren<QQuickItem*>("undirected_graph", Qt::FindChildrenRecursively)) {
            undirected_qangraph = qobject_cast<CustomGraph*>(item);
            break;
        }
        if (undirected_qangraph != nullptr) break;
    }
    for (const auto obj : app_engine.rootObjects()) {
        for (QQuickItem* item :  obj->findChildren<QQuickItem*>("undirected_graphView", Qt::FindChildrenRecursively)) {
            undirected_graphView = qobject_cast<qan::GraphView*>(item);
            break;
        }
        if (undirected_graphView != nullptr) break;
    }
    if (undirected_qangraph == nullptr) {
        qDebug() << "undirected_qangraph not found";
        exit(-1);
    }

    for (const auto obj : app_engine.rootObjects()) {
        for (QQuickItem* item :  obj->findChildren<QQuickItem*>("tab_bar", Qt::FindChildrenRecursively)) {
            tab_bar = item;
            break;
        }
        if (tab_bar != nullptr) break;
    }
    if (tab_bar == nullptr) {
        qDebug() << "tab_bar not found";
        exit(-1);
    }

    for (const auto obj : app_engine.rootObjects()) {
        for (QQuickItem* item :  obj->findChildren<QQuickItem*>("tsp_path_cost", Qt::FindChildrenRecursively)) {
            tsp_solution_cost_label = item;
            break;
        }
        if (tsp_solution_cost_label != nullptr) break;
    }

    for (const auto obj : app_engine.rootObjects()) {
        for (QQuickItem* item :  obj->findChildren<QQuickItem*>("tsp_compute_duration", Qt::FindChildrenRecursively)) {
            tsp_duration_label = item;
            break;
        }
        if (tsp_duration_label != nullptr) break;
    }

    for (const auto obj : app_engine.rootObjects()) {
        for (QQuickItem* item :  obj->findChildren<QQuickItem*>("applied_algorithm", Qt::FindChildrenRecursively)) {
            applied_algorithm_label = item;
            break;
        }
        if (applied_algorithm_label != nullptr) break;
    }

    for (const auto obj : app_engine.rootObjects()) {
        for (QQuickItem* item :  obj->findChildren<QQuickItem*>("node_creation_menu", Qt::FindChildrenRecursively)) {
            node_creation_menu_item = item;
            break;
        }
        if (node_creation_menu_item != nullptr) break;
    }
    if (node_creation_menu_item == nullptr) {
        qDebug() << "couldn\'t load node_creation_menu_item";
        exit(-1);
    }


    undirected_graph = new Graph;
    undirected_graph->setType(Graph::GraphType::Undirected);

    QQuickItem* container = undirected_graphView->getContainerItem();
    graphView_container_changed();
    QObject::connect(container, SIGNAL(xChanged()), this, SLOT(graphView_container_changed()));
    QObject::connect(container, SIGNAL(yChanged()), this, SLOT(graphView_container_changed()));
    QObject::connect(container, SIGNAL(scaleChanged()), this, SLOT(graphView_container_changed()));
//    QObject::connect(undirected_graphView, SIGNAL(scaleChanged()), this, SLOT(zoom_origin_changed()));
    //QObject::connect(undirected_graphView, SIGNAL(zoom_changed()), this, SLOT(zoom_changed()));
    //QObject::connect(undirected_graphView, SIGNAL(navigableChanged()), this, SLOT(undirected_graph_view_navigableChanged()));
}
