#include "abstractionspace.h"
#include <QDebug>

AbstractionSpace::AbstractionSpace()
{
  qDebug() << "Hello abstraction space!";
}

AbstractionSpace::~AbstractionSpace()
{
  qDebug() << "~AbstractionSpace";
}

SelectedVisMethods AbstractionSpace::configureVisMethods(VisConfiguration config)
{
  qDebug() << "Decide on Vis Methods";
  SelectedVisMethods methods;

  methods.low = new BarChart();
  methods.medium = new BarChart();
  methods.high = new BarChart();

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
    methods.low = new DistanceTree();
    methods.medium = new BarChart();
    methods.high = new DistanceMatrix(); // todo change to distance matrix
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
