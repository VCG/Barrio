#include <CGAL/Simple_cartesian.h>
#include <CGAL/centroid.h>
#include <CGAL/AABB_tree.h>
#include <CGAL/AABB_traits.h>
#include <CGAL/AABB_triangle_primitive.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/boost/graph/graph_traits_Polyhedron_3.h>
#include <CGAL/AABB_face_graph_triangle_primitive.h>

#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>

#include <QDomDocument>
#include <QString>
#include <QDebug>

#include "mesh.h"
#include "object.h"
#include "globalParameters.h"

#pragma once
class MeshProcessing
{
public:
	MeshProcessing();
	~MeshProcessing();

	int computeCenter(Object* obj, std::vector<VertexData>* vertices);
	int compute_distance_distribution(Object* mito, Object* cell, std::vector<VertexData>* vertices);
	double compute_closest_distance(Object* mito, Object* synapse, std::vector<VertexData>* vertices);

	float computeSurfaceArea(QVector3D a, QVector3D b, QVector3D c);

private:
	bool isBorderVertex(float x, float y, float z);

};