#include <qwebengineview.h>
#include "javascriptproxy.h"


class InfoVisWidget: public QWebEngineView
{
public:
  InfoVisWidget(QUrl url);

private:
  BarChartSharedData*    m_jsproxy;
};