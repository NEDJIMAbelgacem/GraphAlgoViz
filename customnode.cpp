#include "customnode.h"

QQmlComponent *CustomNode::delegate(QQmlEngine &engine) noexcept
{
  static std::unique_ptr<QQmlComponent> CustomNode_delegate;
  if (!CustomNode_delegate)
    CustomNode_delegate =
        std::make_unique<QQmlComponent>(&engine, "qrc:/CustomNode.qml");
  return CustomNode_delegate.get();
}

qan::NodeStyle *CustomNode::style() noexcept
{
  static std::unique_ptr<qan::NodeStyle> CustomRectNode_style;
  if (!CustomRectNode_style) {
    CustomRectNode_style = std::make_unique<qan::NodeStyle>();
    CustomRectNode_style->setBackColor(QColor("#ff29fc"));
  }
  return CustomRectNode_style.get();
}
