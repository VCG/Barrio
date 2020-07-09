#include "distancetree.h"

DistanceTree::DistanceTree()
{
  QString newickString = "((mito11:0.6, mito12 : 0.78)syn1:1, (mito2 : 2.0, mito3 : 1.2)syn2 : 2, (mito7 : 0.2, mito8 : 0.5, mito9 : 0.4, mito10 : 2.5.0)syn3 : 2.7, (mito4 : 0.3, mito4 : 1.2, mito5 : 0.9, mito6 : 3.0)syn4 : 1.6)mito1;";
  data = new DistanceTreeData(newickString);
}

DistanceTree::~DistanceTree()
{
  // destruct distance tree
}

QWebEngineView* DistanceTree::getVisWidget()
{
  QWebEngineView* view = new QWebEngineView();
  QWebChannel* channel = new QWebChannel(view->page());
  view->page()->setWebChannel(channel);
  channel->registerObject(QString("distance_tree_data"), data);
  view->load(getHTMLPath(m_index_filename));

  return view;
}

QString DistanceTree::createNewickString(int hvgxID, float distanceThreshold)
{
  return QString();
}

DistanceTreeData::DistanceTreeData(QString newickString) 
{
  m_newickString = newickString;
}

DistanceTreeData::~DistanceTreeData()
{
  // do nothing for now
}

Q_INVOKABLE QString DistanceTreeData::getNewickString()
{
  return Q_INVOKABLE m_newickString;
}
