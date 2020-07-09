#ifndef BARCHART_H
#define BARCHART_H

#include <QWebChannel>
#include "vismethod.h"

class BarChartData : public QObject
{
  Q_OBJECT


public:
  BarChartData(QList<QString> labels, QList<float> values);
  ~BarChartData();

  Q_INVOKABLE QList<float> getData();
  Q_PROPERTY(QList<float> values READ getData);

  Q_INVOKABLE QList<QString> getLabels();
  Q_PROPERTY(QList<QString> labels READ getLabels);

  QList<QString> m_labels;
  QList<float> m_values;

};

class BarChart : public IVisMethod
{
public:
  BarChart(DataContainer* datacontainer);
  ~BarChart();

  QWebEngineView* getVisWidget(int ID);

private:
  BarChartData* data;

  DataContainer* m_datacontainer;

  QString m_x_title;
  QString m_y_title;
  QString m_title;

  QString m_index_filename = "barchart_index.html";
};

#endif