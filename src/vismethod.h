#ifndef VISMETHOD_H
#define VISMETHOD_H

#include <QString>
#include <QWebEngineView>
#include <QObject>
#include <QResource>
#include <QFile>

class IVisMethod: public QObject
{
  Q_OBJECT
public:
  virtual QWebEngineView* getVisWidget() = 0;

protected:
  QString m_name;
};

#endif

