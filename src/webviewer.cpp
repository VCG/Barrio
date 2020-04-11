#include "webviewer.h"


MyWebViewer::MyWebViewer()
{
}

MyWebViewer::~MyWebViewer()
{
}

void MyWebViewer::renderWebContent()
{
  /*QWebEngineView* view = new QWebEngineView();
  qDebug() << exePath().c_str();
  view->load(QUrl("C:/Users/jtroidl/Desktop/NeuroComparer/src/web/colorGame.html"));
  view->show();*/
}

std::string MyWebViewer::exePath()
{
  char buffer[1000];
  GetModuleFileName(NULL, buffer, 1000);
  std::string::size_type pos = std::string(buffer).find_last_of("\\/");
  return std::string(buffer).substr(0, pos);
}
