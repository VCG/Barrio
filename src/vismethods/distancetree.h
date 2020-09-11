#ifndef DISTANCETREE_H
#define DISTANCETREE_H

#include <QChar>

#include "vismethod.h"
#include "../globalParameters.h"
#include "../datacontainer.h"

class DistanceTreeData : public QObject 
{
  Q_OBJECT

public:
  DistanceTreeData(int ID, QString newickString, GlobalVisParameters* m_global_vis_parameters, DataContainer* data_container);
  ~DistanceTreeData();

  Q_INVOKABLE QString getNewickString();
  Q_PROPERTY(QString newickString READ getNewickString);
  
  Q_INVOKABLE void setSelectedNode(QString name);
  Q_INVOKABLE QString getSelectedNode();
  Q_PROPERTY(QString new_selected_node READ getSelectedNode);

  Q_INVOKABLE void setHighlightedStructure(const QString& name);
  Q_INVOKABLE void removeHighlightedStructure(const QString& name_to_remove);
  Q_INVOKABLE void removeAllHighlightedStructures();

  void setNewickString(QString newick) { m_newickString = newick; };
  void setHvgxID(int ID) { m_hvgxID = ID; };

  int m_hvgxID;
  QString m_newickString;
  QString m_selected_node;

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
  QWebEngineView* initVisWidget(int ID, SpecificVisParameters params);
  QWebEngineView* getWebEngineView();
  bool            update();
  DistanceTree*   clone();

  VisType         getType();

  void            setSpecificVisParameters(SpecificVisParameters params);

private:

  DistanceTreeData* data;
  DataContainer* m_datacontainer;

  QString m_title;
  QString m_index_filename = "web/distancetree_index.html";

  QString createNewickString(int hvgxID, float distanceThreshold);

  GlobalVisParameters* m_global_vis_parameters;
  QWebEngineView* m_web_engine_view;

  static bool sortByVal(const std::pair<int, double>& a,
    const std::pair<int, double>& b)
  {
    return (a.second < b.second);
  }
};

#endif
