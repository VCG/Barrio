#ifndef DISTANCETREE_H
#define DISTANCETREE_H

#include <QChar>

#include "vismethod.h"

class DistanceTreeData : public QObject 
{
  Q_OBJECT

public:
  DistanceTreeData(int ID, QString newickString, GlobalVisParameters* m_global_vis_parameters, DataContainer* data_container);
  ~DistanceTreeData();

  Q_INVOKABLE QString getNewickString();
  Q_PROPERTY(QString newickString READ getNewickString);

  Q_INVOKABLE void setHighlightedStructure(const QString& name); const

  void setNewickString(QString newick) { m_newickString = newick; };
  void setHvgxID(int ID) { m_hvgxID = ID; };

  int m_hvgxID;
  QString m_newickString;

  GlobalVisParameters* m_global_vis_parameters;
  DataContainer*       m_datacontainer;
};

class DistanceTree : public IVisMethod
{
public:
  DistanceTree(DistanceTree* distanceTree);
  DistanceTree(GlobalVisParameters* visparams, DataContainer* datacontainer);
  ~DistanceTree();

  //inherited functions
  QWebEngineView* initVisWidget(int ID);
  QWebEngineView* getWebEngineView();
  bool            update();
  DistanceTree*   clone();

private:

  DistanceTreeData* data;
  DataContainer* m_datacontainer;

  QString m_title;
  QString m_index_filename = "distancetree_index.html";

  QString createNewickString(int hvgxID, float distanceThreshold);

  GlobalVisParameters* m_global_vis_parameters;
  QWebEngineView* m_web_engine_view;
};

#endif
