#include "customgraph.h"


qan::Group *CustomGraph::insertCustomGroup() {
  return qan::Graph::insertGroup<qan::Group>();
}

qan::Node *CustomGraph::insertCustomNode() {
  return qan::Graph::insertNode<CustomNode>(nullptr);
}

qan::Edge *CustomGraph::insertCustomEdge(qan::Node *source,
                                         qan::Node *destination) {
  const auto engine = qmlEngine(this);
  if (source != nullptr &&
      destination != nullptr &&
      engine != nullptr)
    return qan::Graph::insertEdge<qan::Edge>(*source, destination, qan::Edge::delegate(*engine));
  return nullptr;
}
