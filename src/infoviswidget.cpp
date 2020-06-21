#include "infoviswidget.h"
#include <qwebchannel.h>

InfoVisWidget::InfoVisWidget(QUrl url) 
{
  QWebChannel* channel = new QWebChannel(page());
  page()->setWebChannel(channel);
  // Create proxy object
  MyJavaScriptProxy* data = new MyJavaScriptProxy(5);
  // Register object
  channel->registerObject(QString("test"), data);

  load(url);
}


