#pragma once
#include <QWebEngineView>
#include <string>
#include <windows.h>

class MyWebViewer
{
public:
  MyWebViewer();
  ~MyWebViewer();

  void renderWebContent();
  std::string exePath();

protected:
  int hello;
};