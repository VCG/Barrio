#ifndef BARCHART_H
#define BARCHART_H

#include <QWebChannel>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include "vismethod.h"

class BarChartData : public QObject
{
  Q_OBJECT


public:
  BarChartData(QString json_string);
  ~BarChartData();

  Q_INVOKABLE QString getData();
  Q_PROPERTY(QString json_string READ getData);

  QString m_json_string;

};

class BarChart : public IVisMethod
{
public:
  BarChart(DataContainer* datacontainer);
  ~BarChart();

  QWebEngineView* initVisWidget(int ID);
  bool            update();
  QWebEngineView* getWebEngineView();
  BarChart*       clone();

  QString         createJSONString(QList<int>* selected_mitos, double distance_threshold);

private:
  BarChartData* data;

  DataContainer* m_datacontainer;

  QString m_x_title;
  QString m_y_title;
  QString m_title;

  QString m_index_filename = "barchart_index.html";
  QWebEngineView* m_web_engine_view;
};

#endif