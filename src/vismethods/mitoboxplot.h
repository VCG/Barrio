#ifndef MITOBOXPLOT_H
#define MITOBOXPLOT_H

#include <QChar>
#include "vismethod.h"
#include "../globalParameters.h"
#include "../datacontainer.h"

class MitoBoxPlotData : public QObject
{
  Q_OBJECT

public:
  MitoBoxPlotData(QString json, DataContainer* data_container, GlobalVisParameters* global_vis_parameters);
  ~MitoBoxPlotData();

  Q_INVOKABLE QString getData();
  Q_PROPERTY(QString json_string READ getData);

  void setJSONString(QString json) { m_json_string = json; };

  int m_hvgxID;
  QString m_json_string;

  GlobalVisParameters* m_global_vis_parameters;
  DataContainer* m_datacontainer;

  Q_INVOKABLE void setHighlightedStructure(const int parentID, int spineNumber);
  Q_INVOKABLE void removeHighlightedStructure(const int parentID, int spineNumber);
};

class MitoBoxPlot : public IVisMethod
{
public:
  MitoBoxPlot(MitoBoxPlot* mitoBoxPlot);
  MitoBoxPlot(GlobalVisParameters* visparams, DataContainer* datacontainer);
  ~MitoBoxPlot();

  QString createJSONString(DataContainer* data_container, QList<int>* selectedObjects);

  //inherited functions
  QWebEngineView* initVisWidget(int ID, SpecificVisParameters params);
  QWebEngineView* getWebEngineView();
  bool            update();
  MitoBoxPlot* clone();

  VisType         getType();

  void            setSpecificVisParameters(SpecificVisParameters params);

private:

  MitoBoxPlotData* m_data;
  DataContainer* m_datacontainer;

  QString m_title;
  QString m_index_filename = "web/mitoboxplot_index.html";

  GlobalVisParameters* m_global_vis_parameters;
  QWebEngineView* m_web_engine_view;
};

#endif
