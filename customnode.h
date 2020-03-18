#ifndef CUSTOMNODE_H
#define CUSTOMNODE_H
// Qt headers
#include <QGuiApplication>
#include <QtQml>
#include <QQuickStyle>

// QuickQanava headers
#include <QuickQanava.h>

class CustomNode : public qan::Node
{
  Q_OBJECT
public:
  explicit CustomNode(QObject* parent = nullptr) : qan::Node{parent} { /* Nil */ }
  virtual ~CustomNode() override { /* Nil */ }
private:
  CustomNode(const CustomNode &) = delete;

public:
  static QQmlComponent*     delegate(QQmlEngine &engine) noexcept;
  static qan::NodeStyle*    style() noexcept;
};

QML_DECLARE_TYPE(CustomNode)


#endif // CUSTOMNODE_H
