#include "infoviswidget.h"
#include <qwebchannel.h>

InfoVisWidget::InfoVisWidget(QUrl url) 
{
  QWebChannel* channel = new QWebChannel(page());
  page()->setWebChannel(channel);
  // Create proxy object
  BarChartSharedData* data = new BarChartSharedData();
  // Register object
  channel->registerObject(QString("data"), data);

  load(url);
}


