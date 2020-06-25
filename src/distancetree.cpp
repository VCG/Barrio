#include "distancetree.h"

DistanceTree::DistanceTree()
{
  // setup distance tree
}

DistanceTree::~DistanceTree()
{
  // destruct distance tree
}

Q_INVOKABLE QList<float> DistanceTree::getSynapseDistances()
{
  return Q_INVOKABLE m_synapse_distances;
}

Q_INVOKABLE QList<QString> DistanceTree::getSynapseNames()
{
  return Q_INVOKABLE m_synapse_names;
}

QWebEngineView* DistanceTree::getVisWidget()
{
  QWebEngineView* view = new QWebEngineView();
  QWebChannel* channel = new QWebChannel(view->page());
  view->page()->setWebChannel(channel);
  channel->registerObject(QString("data"), this);
  view->load(getHTMLPath(m_index_filename));

  return view;
}
