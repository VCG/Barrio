#ifndef DISTANCETREE_H
#define DISTANCETREE_H

#include <QWebChannel>
#include "vismethod.h"

class DistanceTree : public IVisMethod
{
public:
  DistanceTree();
  ~DistanceTree();

  Q_INVOKABLE QList<float> getSynapseDistances();
  Q_PROPERTY(QList<float> synapseDistances READ getSynapseDistances);

  Q_INVOKABLE QList<QString> getSynapseNames();
  Q_PROPERTY(QList<QString> synapseNames READ getSynapseNames);

  QWebEngineView* getVisWidget();

private:
  QList<QString> m_synapse_names;
  QList<float> m_synapse_distances;

  QString m_title;

  QString m_index_filename = "distancetree_index.html";
};

#endif
