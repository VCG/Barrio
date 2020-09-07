#include "abstractionspace.h"
#include <QDebug>

AbstractionSpace::AbstractionSpace(DataContainer* datacontainer)
{
  m_datacontainer = datacontainer;
  
  m_global_vis_parameters.distance_threshold = 1.0;
  m_global_vis_parameters.opacity = 0.0;
  m_global_vis_parameters.slice_depth = 0.0;
}

AbstractionSpace::~AbstractionSpace()
{
  qDebug() << "~AbstractionSpace";
}

SelectedVisMethods AbstractionSpace::configureVisMethods(VisConfiguration config)
{
  qDebug() << "Decide on Vis Methods";
  SelectedVisMethods methods;

  methods.low = new BarChart(&m_global_vis_parameters, m_datacontainer);
  methods.low_icon = ":/icons/barchart.png";
  methods.medium = new BarChart(&m_global_vis_parameters, m_datacontainer);
  methods.medium_icon = ":/icons/barchart.png";
  methods.high = new BarChart(&m_global_vis_parameters, m_datacontainer);
  methods.high_icon = ":/icons/barchart.png";

  if (config.axons && !config.dends && !config.mitos && !config.syn) 
  {
    // low - skeleton
    // medium - scatterplot
    // high - scatterplot
    qDebug() << "Hello world";
  }
  else if (!config.axons && config.dends && !config.mitos && !config.syn)
  {
    // low - skeleton
    // medium - scatterplot
    // high - scatterplot
    qDebug() << "Hello world";
  }
  else if (!config.axons && !config.dends && config.mitos && !config.syn)
  {
    // low - skeleton
    // medium - todo
    // high - scatterplot
    qDebug() << "Hello world";
  }
  else if (!config.axons && !config.dends && !config.mitos && config.syn)
  {
    // low - 3D view
    // medium - barchart
    // high - barchart
    qDebug() << "Hello world";
  }
  else if (config.axons && config.dends && !config.mitos && !config.syn)
  {
    qDebug() << "Hello world";
  }
  else if (config.axons && !config.dends && config.mitos && !config.syn)
  {
    //UT 3
    methods.low = new Histogram(&m_global_vis_parameters, m_datacontainer);
    methods.low_icon = ":/icons/histogram.png";
    methods.medium = new Boxplot(&m_global_vis_parameters, m_datacontainer);
    methods.medium_icon = ":/icons/boxplot.png";
    methods.high = new Scatterplot(&m_global_vis_parameters, m_datacontainer);
    methods.high_icon = ":/icons/scatterplot.png";


  }
  else if (config.axons && !config.dends && !config.mitos && config.syn)
  {
    qDebug() << "Hello world";
  }
  else if (!config.axons && config.dends && config.mitos && config.syn)
  {
    // low: distance tree
    // medium: barchart
    // high: distance matrix
    methods.low = new DistanceTree(&m_global_vis_parameters, m_datacontainer);
    methods.medium = new BarChart(&m_global_vis_parameters, m_datacontainer);
    methods.high = new DistanceMatrix(&m_global_vis_parameters, m_datacontainer); // todo change to distance matrix
    
    methods.low_icon = ":/icons/distance_tree.png";
    methods.medium_icon = ":/icons/barchart.png";
    methods.high_icon = ":/icons/matrix.png";
  }
  else if (!config.axons && config.dends && !config.mitos && config.syn)
  {
    qDebug() << "Hello world";
  }
  else if (!config.axons && !config.dends && config.mitos && config.syn)
  {
    qDebug() << "Hello world";
  }
  else if (config.axons && config.dends && config.mitos && !config.syn)
  {
    methods.low = new Histogram(&m_global_vis_parameters, m_datacontainer);
    methods.low_icon = ":/icons/histogram.png";
    methods.medium = new Boxplot(&m_global_vis_parameters, m_datacontainer);
    methods.medium_icon = ":/icons/boxplot.png";
    methods.high = new Scatterplot(&m_global_vis_parameters, m_datacontainer);
    methods.high_icon = ":/icons/scatterplot.png";
  }
  else if (!config.axons && config.dends && config.mitos && !config.syn)
  {
    methods.low = new Histogram(&m_global_vis_parameters, m_datacontainer);
    methods.low_icon = ":/icons/histogram.png";
    methods.medium = new Boxplot(&m_global_vis_parameters, m_datacontainer);
    methods.medium_icon = ":/icons/boxplot.png";
    methods.high = new Scatterplot(&m_global_vis_parameters, m_datacontainer);
    methods.high_icon = ":/icons/scatterplot.png";
  }
  else if (config.axons && config.dends && !config.mitos && config.syn)
  {
    qDebug() << "Hello world";
  }
  else if (config.axons && config.dends && config.mitos && config.syn)
  {
    qDebug() << "Hello world";
  }
 
  return methods;
}
