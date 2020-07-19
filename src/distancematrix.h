#ifndef DISTANCEMATRIX_H
#define DISTANCEMATRIX_H

#include "vismethod.h"

class DistanceMatrixData : public QObject
{
  Q_OBJECT

public:
  DistanceMatrixData(QString json_string);
  ~DistanceMatrixData();

  Q_INVOKABLE QString getData();
  Q_PROPERTY(QString json_string READ getData);

  void setJsonString(QString json) { m_json_string = json; }
  QString getJsonString() { return m_json_string; }

  QString m_json_string;

};

class DistanceMatrix : public IVisMethod
{
public:
  DistanceMatrix(DistanceMatrix* matrix);
  DistanceMatrix(DataContainer* datacontainer);
  ~DistanceMatrix();

  QWebEngineView* initVisWidget(int ID);
  bool            update();
  QWebEngineView* getWebEngineView();
  DistanceMatrix* clone();

private:
  DistanceMatrixData* data;

  DataContainer* m_datacontainer;
  QString m_title;

  QString m_index_filename = "distancematrix_index.html";
  QWebEngineView* m_web_engine_view;

  QString getJSONString(QList<int>* selected_mitos);
};

#endif
