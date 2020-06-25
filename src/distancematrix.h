#ifndef DISTANCEMATRIX_H
#define DISTANCEMATRIX_H

#include <QWebChannel>
#include "vismethod.h"

class DistanceMatrix : public IVisMethod
{
public:
  DistanceMatrix();
  ~DistanceMatrix();

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

  QString m_index_filename = "distancematrix_index.html";
};

#endif
