#include "javascriptproxy.h"

MyJavaScriptProxy::MyJavaScriptProxy(int data, QObject* parent) : QObject(parent)
{
  //setObjectName("m_QtProxy");
  m_data = data;
}

Q_INVOKABLE int MyJavaScriptProxy::getData()
{
  return Q_INVOKABLE m_data;
}
