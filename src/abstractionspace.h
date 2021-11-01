#ifndef ABSTRACTIONSPACE_H
#define ABSTRACTIONSPACE_H

#include <vector>
#include <string>
#include <QVector4D>

#include "mainopengl.h"
#include "ssbo_structs.h"
#include "datacontainer.h"

#include "vismethods/vismethod.h"
#include "vismethods/distancetree.h"
#include "vismethods/barchart.h"
#include "vismethods/distancematrix.h"
#include "vismethods/histogram.h"
#include "vismethods/boxplot.h"
#include "vismethods/scatterplot.h"
#include "vismethods/mito_scheme.h"
#include "vismethods/mitoboxplot.h"


struct AbstractionPoint {
	QVector2D point;
	int       ID;
};

enum VisName { MyDistanceTree, MyBarChart, MyDistanceMatrix, MyHistogram, MyViolinChart, MyScatterPlot, MySkeleton, MyMitoBoxPlot, MyMitoScatterPlot };

struct Vis
{
	VisName name;
	QString icon_path;
	int id;
	NumberOfEntities scale;
};

struct VisConfiguration
{
	bool axons;
	bool dends;
	bool mitos;
	bool syn;

	bool sliceView;
};

struct SelectedVisMethods
{
	IVisMethod* method;
};

class AbstractionSpace : public MainOpenGL
{
public:

	std::map<int, Vis> m_vis_space;

	AbstractionSpace(DataContainer* datacontainer);
	~AbstractionSpace();

	IVisMethod* decideOnVisMethod(Vis vis);

	std::vector<struct AbstractionPoint> get2DSpaceVertices() { return m_vertices; }
	std::vector<GLuint> get2DSpaceIndices() { return m_indices; }

	std::vector<struct AbstractionPoint> get2DSpaceGridVertices() { return m_grid_vertices; }
	std::vector<GLuint> get2DSpaceGridIndices() { return m_grid_indices; }
	std::vector<struct AbstractionPoint> get2DSpaceGridIlligalIndices() { return m_grid_illigal_vertices; }

	struct ast_neu_properties getSpaceProper() { return m_IntervalXY[m_intervalID]; }

	void setThresholdDistance(double distance) { m_global_vis_parameters.distance_threshold = distance; }
	double getThresholdDistance() { return m_global_vis_parameters.distance_threshold; }

	void setCellOpacity(float opacity) { m_global_vis_parameters.opacity = opacity; }
	float getCellOpacity() { return m_global_vis_parameters.opacity; }

	void setSliceDepth(float depth) { m_global_vis_parameters.slice_depth = depth; }
	float getSliceDepth() { return m_global_vis_parameters.slice_depth; }

	void addToSelectedIndices(int id)
	{
		if (!m_global_vis_parameters.selected_objects.contains(id))
		{
			m_global_vis_parameters.selected_objects.append(id);
		}
	}

	void removeFromSelectedindices(int id)
	{
		if (m_global_vis_parameters.selected_objects.contains(id))
		{
			m_global_vis_parameters.selected_objects.removeAll(id);
		}
	}

	QList<int>* getSelectedIndexList() { return &m_global_vis_parameters.selected_objects; }

	DataContainer* m_datacontainer;
	GlobalVisParameters m_global_vis_parameters;

private:
	struct pair_hash {
		template <class T1, class T2>
		std::size_t operator () (const std::pair<T1, T2>& p) const {
			auto h1 = std::hash<T1>{}(p.first);
			auto h2 = std::hash<T2>{}(p.second);
			return h1 ^ h2;
		}
	};

	int                                         m_xaxis;
	int                                         m_yaxis;

	int                                         m_intervalID;
	// OpenGL
	GLuint                                      m_buffer;
	GLuint                                      m_bindIdx;
	bool                                        m_glFunctionsSet;

	// Datas
	struct ssbo_2DState* m_2DState;

	std::vector< struct ast_neu_properties >    m_IntervalXY;

	std::vector<struct AbstractionPoint>        m_vertices;
	std::vector<GLuint>                         m_indices;
	std::map<std::pair<int, int>, struct properties>
		m_neu_states;
	std::map<std::pair<int, int>, struct properties>
		m_ast_states;

	std::vector<struct AbstractionPoint>        m_grid_vertices;
	std::vector<GLuint>                         m_grid_indices;
	std::vector<struct AbstractionPoint>        m_grid_illigal_vertices;
};

#endif // ABSTRACTIONSPACE_H
