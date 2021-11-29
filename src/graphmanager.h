// this should only handle the 4 graphs, their construction, layouting algorithms
// and updating their data, and drawing

#ifndef GRAPHMANAGER_H
#define GRAPHMANAGER_H

#include "graph.h"
#include "datacontainer.h"
#include "glsluniform_structs.h"
#include <thread>

// (1) neurite-neurite graph
// (2) neurite-astrocyte skeleton
// (3) neurites skeletons - astrocyte skeleton
// (4) neuries skeletons
#define max_graphs 4

class GraphManager
{
public:
	GraphManager(DataContainer* objectManager);
	~GraphManager();

	void ExtractGraphFromMesh();

	void drawNeuritesGraph();
	void drawSkeletonsGraph();

	void updateUniformsLocation(GLuint program);
	void updateUniforms(struct WidgetUniforms graph_uniforms);

	void initNeuritesVertexAttribPointer();
	void initSkeletonsVertexAttribPointer();

	// force directed layout
	void stopForceDirectedLayout(int graphIdx);

	void update2Dflag(bool is2D, struct WidgetUniforms uniforms);

	struct FDR_param getFDRParams(int graph_index) { return m_graph[graph_index]->getFDRParams(); }
	void updateFDRParamts(int graph_index, struct FDR_param params) { m_graph[graph_index]->updateFDRParamts(params); }

protected:
	DataContainer* m_data_containter;
	Graph* m_graph[max_graphs];

	// thread management
	std::thread                         m_layout_threads[max_graphs];
};

#endif // GRAPHMANAGER_H
