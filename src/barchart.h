#ifndef BARCHART_H
#define BARCHART_H

#include <QWebChannel>
#include "vismethod.h"

class Barchart : public IVisMethod
{
public:
  Barchart();
  ~Barchart();

  Q_INVOKABLE QList<float> getData();
  Q_PROPERTY(QList<float> values READ getData);

  Q_INVOKABLE QList<QString> getLabels();
  Q_PROPERTY(QList<QString> labels READ getLabels);

  QWebEngineView* getVisWidget();

private:
  QList<QString> m_labels;
  QList<float> m_values;

  QString m_x_title;
  QString m_y_title;
  QString m_title;

  const char* m_index_url = ":/barchart_index.html";
};

#endif