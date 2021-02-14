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
  ScatterplotData(QString json, QString selected_objects_json, GlobalVisParameters* global_vis_parameters, DataContainer* data_container);
  ~ScatterplotData();

  Q_INVOKABLE QString getData();
  Q_INVOKABLE QString getXAxis();
  Q_INVOKABLE QString getYAxis();
  Q_INVOKABLE QString getXUnit();
  Q_INVOKABLE QString getYUnit();
  Q_INVOKABLE QString getSelectedStructures();

  Q_INVOKABLE void selectStructure(const QString& name);
  Q_INVOKABLE void removeStructure(const QString& name);

  Q_PROPERTY(QString json_string READ getData);
  Q_PROPERTY(QString x_axis_name READ getXAxis());
  Q_PROPERTY(QString y_axis_name READ getYAxis());
  Q_PROPERTY(QString x_unit READ getXUnit());
  Q_PROPERTY(QString y_unit READ getYUnit());
  Q_PROPERTY(QString selected_structures READ getSelectedStructures());

  Q_INVOKABLE void setHighlightedFrame(const QString& name);
  Q_INVOKABLE void removeHighlightedFrame(const QString& name_to_remove);

  void setJSONString(QString json) { m_json_string = json; };
  void setSelectedStructures(QString structures) { m_selected_structures_json = structures; };

  int m_hvgxID;
  QString m_json_string;
  QString m_selected_structures_json; 

  GlobalVisParameters* m_global_vis_parameters;
  DataContainer* m_datacontainer;
};

class Scatterplot : public IVisMethod
{
public:
  Scatterplot(Scatterplot* scatterplot);
  Scatterplot(GlobalVisParameters* visparams, DataContainer* datacontainer);
  ~Scatterplot();

  QString createJSONString();
  QString createSelectedObjectsJSON(QList<int>* selectedObjects);

  //inherited functions
  QWebEngineView* initVisWidget(int ID, SpecificVisParameters params);
  QWebEngineView* getWebEngineView();
  bool            update();
  Scatterplot*    clone();
  bool            update_needed();

  VisType         getType();

  void            setSpecificVisParameters(SpecificVisParameters params);

private:

  ScatterplotData* m_data;
  DataContainer* m_datacontainer;

  QString m_title;
  QString m_index_filename = "web/scatter.html";

  GlobalVisParameters* m_global_vis_parameters;
  QWebEngineView* m_web_engine_view;
};

#endif

