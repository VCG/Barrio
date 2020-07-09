#include "abstractionspace.h"
#include <QDebug>

AbstractionSpace::AbstractionSpace(DataContainer* datacontainer)
{
  qDebug() << "Hello abstraction space!";
  m_datacontainer = datacontainer;
}

AbstractionSpace::~AbstractionSpace()
{
  qDebug() << "~AbstractionSpace";
}

SelectedVisMethods AbstractionSpace::configureVisMethods(VisConfiguration config)
{
  qDebug() << "Decide on Vis Methods";
  SelectedVisMethods methods;

  methods.low = new BarChart(m_datacontainer);
  methods.medium = new BarChart(m_datacontainer);
  methods.high = new BarChart(m_datacontainer);

  if (config.axons && !config.dends && !config.mitos && !config.syn) 
  {
    // low - skeleton
    // medium - scatterplot
    // high - scatterplot
  }
  else if (!config.axons && config.dends && !config.mitos && !config.syn)
  {
    // low - skeleton
    // medium - scatterplot
    // high - scatterplot
  }
  else if (!config.axons && !config.dends && config.mitos && !config.syn)
  {
    // low - skeleton
    // medium - todo
    // high - scatterplot
  }
  else if (!config.axons && !config.dends && !config.mitos && config.syn)
  {
    // low - 3D view
    // medium - barchart
    // high - barchart
  }


  else if (config.axons && config.dends && !config.mitos && !config.syn)
  {

  }
  else if (config.axons && !config.dends && config.mitos && !config.syn)
  {

  }
  else if (config.axons && !config.dends && !config.mitos && config.syn)
  {

  }


  else if (!config.axons && config.dends && config.mitos && config.syn)
  {
    // low: distance tree
    // medium: barchart
    // high: distance matrix
    methods.low = new DistanceTree(m_datacontainer);
    methods.medium = new BarChart(m_datacontainer);
    methods.high = new DistanceMatrix(m_datacontainer); // todo change to distance matrix
  }
  else if (!config.axons && config.dends && !config.mitos && config.syn)
  {

  }
  else if (!config.axons && !config.dends && config.mitos && config.syn)
  {

  }


  else if (config.axons && config.dends && config.mitos && !config.syn)
  {

  }
  else if (config.axons && config.dends && !config.mitos && config.syn)
  {

  }

  else if (config.axons && config.dends && config.mitos && config.syn)
  {

  }
 
  return methods;
}
