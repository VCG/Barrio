#ifndef SCATTERPLOT_H
#define SCATTERPLOT_H

#include <QChar>
#include "vismethod.h"
#include "../globalParameters.h"
#include "../datacontainer.h"

class ScatterplotData : public QObject
{
  Q_OBJECT

public:
  ScatterplotData(QString json, GlobalVisParameters* global_vis_parameters, DataContainer* data_container);
  ~ScatterplotData();

  Q_INVOKABLE QString getData();
  Q_PROPERTY(QString json_string READ getData);

  void setJSONString(QString json) { m_json_string = json; };

  int m_hvgxID;
  QString m_json_string;

  GlobalVisParameters* m_global_vis_parameters;
  DataContainer* m_datacontainer;
};

class Scatterplot : public IVisMethod
{
public:
  Scatterplot(Scatterplot* scatterplot);
  Scatterplot(GlobalVisParameters* visparams, DataContainer* datacontainer);
  ~Scatterplot();

  QString createJSONString(QList<int>* selectedObjects);

  //inherited functions
  QWebEngineView* initVisWidget(int ID, SpecificVisParameters params);
  QWebEngineView* getWebEngineView();
  bool            update();
  Scatterplot*    clone();

  VisType         getType();

  void            setSpecificVisParameters(SpecificVisParameters params);

private:

  ScatterplotData* m_data;
  DataContainer* m_datacontainer;

  QString m_title;
  QString m_index_filename = "web/scatterplot_index.html";

  GlobalVisParameters* m_global_vis_parameters;
  QWebEngineView* m_web_engine_view;
};

#endif

