#ifndef JAVASCRIPTPROXY_H
#define JAVASCRIPTPROXY_H

#include <QObject>

class MyJavaScriptProxy : public QObject
{
  Q_OBJECT

public:
  MyJavaScriptProxy(int data, QObject* parent = nullptr);

  Q_INVOKABLE int getData();
  Q_PROPERTY(int myIntCppSide READ getData);


private:
  int m_data;
};

#endif