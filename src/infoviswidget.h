#ifndef INFOVISWIDGET_H
#define INFOVISWIDGET_H


#include <qwebengineview.h>
#include "javascriptproxy.h"


class InfoVisWidget: public QWebEngineView
{
public:
  InfoVisWidget(QUrl url);

private:
  BarChartSharedData*    m_jsproxy;
};

#endif