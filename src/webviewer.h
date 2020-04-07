#pragma once
#include <QWebEngineView>

class MyWebViewer
{
public:
  MyWebViewer();
  ~MyWebViewer();

  void renderWebContent();

protected:
  int hello;
};