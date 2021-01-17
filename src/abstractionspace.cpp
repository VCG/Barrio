#include "abstractionspace.h"
#include <QDebug>

AbstractionSpace::AbstractionSpace(DataContainer* datacontainer)
{
  m_datacontainer = datacontainer;
  
  m_global_vis_parameters.distance_threshold = 1.0;
  m_global_vis_parameters.opacity = 0.0;
  m_global_vis_parameters.slice_depth = 0.0;

  
  m_vis_space.insert(std::make_pair<int, Vis>(0, { "Distance Tree", ":/icons/distance_tree.png", 0, 0 }) );
  m_vis_space.insert(std::make_pair<int, Vis>(1, { "Bar Chart", ":/icons/barchart.png", 1, 1 }));
  m_vis_space.insert(std::make_pair<int, Vis>(2, { "Distance Matrix", ":/icons/matrix.png", 2, 2 } ));

  m_vis_space.insert({ 3, { "Histogram", ":/icons/histogram.png", 3, 0 } });
  m_vis_space.insert({ 4, { "Violin Chart", ":/icons/boxplot.png", 4, 1 } });
  m_vis_space.insert({ 5, { "Scatter Plot", ":/icons/scatterplot.png", 5, 2 } });

  m_vis_space.insert({ 6, { "Distance Tree", ":/icons/distance_tree.png", 6, 0 } });
  m_vis_space.insert({ 7, { "Distance Tree", ":/icons/distance_tree.png", 7, 1 } });
  m_vis_space.insert({ 8, { "Distance Tree", ":/icons/distance_tree.png", 8, 2 } });

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
    methods.low = new MitoScheme(&m_global_vis_parameters, m_datacontainer);
    methods.low_icon = ":/icons/histogram.png";
    methods.medium = new Boxplot(&m_global_vis_parameters, m_datacontainer);
    methods.medium_icon = ":/icons/boxplot.png";
    methods.high = new Scatterplot(&m_global_vis_parameters, m_datacontainer);
    methods.high_icon = ":/icons/scatterplot.png";
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
  else if (config.axons && !config.dends && config.mitos && config.syn)
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
    //methods.low = new Histogram(&m_global_vis_parameters, m_datacontainer);
    methods.low = new MitoScheme(&m_global_vis_parameters, m_datacontainer);
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
 
  return methods;
}
