#include "abstractionspace.h"
#include <QDebug>

AbstractionSpace::AbstractionSpace(DataContainer* datacontainer)
{
	m_datacontainer = datacontainer;

	m_global_vis_parameters.distance_threshold = 1.0;
	m_global_vis_parameters.opacity = 0.0;
	m_global_vis_parameters.slice_depth = 0.0;


	m_vis_space.insert(std::make_pair<int, Vis>(0, { VisName::MyDistanceTree, ":/icons/distance_tree.png", 0, NumberOfEntities::LOW }));
	m_vis_space.insert(std::make_pair<int, Vis>(1, { VisName::MyBarChart , ":/icons/barchart.png", 1, NumberOfEntities::MEDIUM }));
	m_vis_space.insert(std::make_pair<int, Vis>(2, { VisName::MyDistanceMatrix, ":/icons/matrix.png", 2, NumberOfEntities::HIGH }));

	m_vis_space.insert({ 3, { VisName::MyHistogram, ":/icons/histogram.png", 3, NumberOfEntities::LOW } });
	m_vis_space.insert({ 4, { VisName::MyViolinChart, ":/icons/violin.png", 4, NumberOfEntities::MEDIUM } });
	m_vis_space.insert({ 5, { VisName::MyScatterPlot, ":/icons/scatterplot.png", 5, NumberOfEntities::HIGH } });

	m_vis_space.insert({ 6, { VisName::MySkeleton, ":/icons/skeleton.png", 6, NumberOfEntities::LOW } });
	m_vis_space.insert({ 7, { VisName::MyMitoBoxPlot, ":/icons/boxplot.png", 7, NumberOfEntities::MEDIUM } });
	m_vis_space.insert({ 8, { VisName::MyMitoScatterPlot, ":/icons/scatterplot.png", 8, NumberOfEntities::HIGH } });

}

AbstractionSpace::~AbstractionSpace()
{
	qDebug() << "~AbstractionSpace";
}

IVisMethod* AbstractionSpace::decideOnVisMethod(Vis vis)
{
	switch (vis.name)
	{
	case MyDistanceTree:
		return new DistanceTree(&m_global_vis_parameters, m_datacontainer);
	case MyBarChart:
		return new BarChart(&m_global_vis_parameters, m_datacontainer);
	case MyDistanceMatrix:
		return new DistanceMatrix(&m_global_vis_parameters, m_datacontainer);
	case MyHistogram:
		return new Histogram(&m_global_vis_parameters, m_datacontainer);
	case MyViolinChart:
		return new Boxplot(&m_global_vis_parameters, m_datacontainer);
	case MyScatterPlot:
		return new Scatterplot(&m_global_vis_parameters, m_datacontainer);
	case MySkeleton:
		return new MitoScheme(&m_global_vis_parameters, m_datacontainer);
	case MyMitoBoxPlot:
		return new MitoBoxPlot(&m_global_vis_parameters, m_datacontainer);
	case MyMitoScatterPlot:
		return new Scatterplot(&m_global_vis_parameters, m_datacontainer);
	}
}

Vis AbstractionSpace::getVisMethod(int vis_id)
{
	return  m_vis_space.at(vis_id);
}
