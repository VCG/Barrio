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

#pragma once
class MeshProcessing
{
public:
  MeshProcessing();
  ~MeshProcessing();

  int computeCenters(QString obj_path);
  int computeDistance(QString astro_obj_path, QString neurites_obj_path);
};