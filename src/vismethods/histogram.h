#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <QChar>
#include "vismethod.h"
#include "../globalParameters.h"
#include "../datacontainer.h"

class HistogramData : public QObject
{
  Q_OBJECT

public:
  HistogramData(int ID, GlobalVisParameters* m_global_vis_parameters, DataContainer* data_container);
  ~HistogramData();

  /*Q_INVOKABLE QString getNewickString();
  Q_PROPERTY(QString newickString READ getNewickString);

  Q_INVOKABLE void setSelectedNode(QString name);
  Q_INVOKABLE QString getSelectedNode();
  Q_PROPERTY(QString new_selected_node READ getSelectedNode);

  Q_INVOKABLE void setHighlightedStructure(const QString& name);
  Q_INVOKABLE void removeHighlightedStructure(const QString& name_to_remove);
  Q_INVOKABLE void removeAllHighlightedStructures();

  void setNewickString(QString newick) { m_newickString = newick; };
  void setHvgxID(int ID) { m_hvgxID = ID; };*/

  int m_hvgxID;
  QString m_json_string;

  GlobalVisParameters* m_global_vis_parameters;
  DataContainer* m_datacontainer;
};

class Histogram : public IVisMethod
{
public:
  Histogram(Histogram* distanceTree);
  Histogram(GlobalVisParameters* visparams, DataContainer* datacontainer);
  ~Histogram();

  //inherited functions
  QWebEngineView* initVisWidget(int ID);
  QWebEngineView* getWebEngineView();
  bool            update();
  Histogram*      clone();

private:

  HistogramData* data;
  DataContainer* m_datacontainer;

  QString m_title;
  QString m_index_filename = "web/histogram_index.html";

  GlobalVisParameters* m_global_vis_parameters;
  QWebEngineView* m_web_engine_view;
};

#endif

