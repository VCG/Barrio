#include "mesh_preprocessing.h"


typedef CGAL::Simple_cartesian<double>              k;
using point = k::Point_3;
typedef k::Triangle_3                               triangle;
typedef std::list<triangle>::iterator               iterator;
typedef CGAL::AABB_triangle_primitive<k, iterator>  primitive;
typedef CGAL::AABB_traits<k, primitive>             aabb_triangle_traits;
typedef CGAL::AABB_tree<aabb_triangle_traits>       tree;

MeshProcessing::MeshProcessing()
{
  qDebug() << "Init Mesh Preprocessor";
}

MeshProcessing::~MeshProcessing()
{
  qDebug() << "Destroy Mesh Preprocessor";
}

int MeshProcessing::computeCenters(QString obj_path)
{
  FILE* fp = fopen(obj_path.toStdString().c_str(), "r");
  if (!fp) return 0;
  char line[1024];

  std::vector<point> objects;
  std::vector<triangle> triangles;

  std::map<int, QVector4D> centers; // maps hvgx id to geometrical center
  int hvgxID = 0;

  double x, y, z;
  int f1, f2, f3, n1, n2, n3;
  int flag = 0;

  while (fgets(line, 1024, fp)) {
    // read obj vertices
    if (line[0] == 'o') {
      // get center for last object
      if (flag == 1 && triangles.size() > 1) {
        point c = CGAL::centroid(triangles.begin(), triangles.end(), CGAL::Dimension_tag<2>());
        QVector3D center(c.x(), c.y(), c.z());
        centers[hvgxID] = center;
      }
      
      // extract hvgx id out of name
      QString nameline(line);
      QList<QString> nameList = nameline.split('_');
      hvgxID = nameList.last().toInt();

      triangles.clear();
      flag = 1;
    }
    else if (line[0] == 'v') {
      int result = sscanf(line, "%*s%lf%lf%lf", &x, &y, &z);
      point p(x, y, z);
      objects.push_back(p);
    }
    else if (line[0] == 'f') {
      int result = sscanf(line, "f %d//%d %d//%d %d//%d\n", &f1, &n1, &f2, &n2, &f3, &n3);
      triangles.push_back(triangle(objects[f1], objects[f2], objects[f3]));
    }
  }

  point c = CGAL::centroid(triangles.begin(), triangles.end(), CGAL::Dimension_tag<2>());
  QVector3D center(c.x(), c.y(), c.z());
  centers[hvgxID] = center;

  return EXIT_SUCCESS;
}

struct Astrocyte {
  std::string name;
  std::list<triangle> triangles;
  std::vector<point> vertices;
};

// Load astrocyte mesh and build its tree
// Load other objects (neurites)
// for each other object, check against the astrocyte, and get the closest vertices from neurites to skeleton mesh
// Goal: find all closest points to astrocyte

int MeshProcessing::compute_distance(Object* mito, Object* cell, std::vector<VertexData>* vertices) const
{
  struct Astrocyte my_cell;
  my_cell.name = cell->getName();
 
  std::vector<int>* cell_indices = cell->get_indices_list();

  // extract vertex data
  for (auto i = 0; i < cell_indices->size(); i = i + 3)
  {
    // vertex 0
    const int idx_0 = (*cell_indices)[i]; // dereferencing pointer
    auto v0 = vertices->at(idx_0);
    point p0(v0.x(), v0.y(), v0.z());
    my_cell.vertices.push_back(p0);

    // vertex 1
    const int idx_1 = (*cell_indices)[i + 1]; // dereferencing pointer
    auto v1 = vertices->at(idx_1);
    point p1(v1.x(), v1.y(), v1.z());
    my_cell.vertices.push_back(p1);

    // vertex 2
    const int idx_2 = (*cell_indices)[i + 2]; // dereferencing pointer
    auto v2 = vertices->at(idx_2);
    point p2(v2.x(), v2.y(), v2.z());
    my_cell.vertices.push_back(p2);

    // add face
    my_cell.triangles.emplace_back(p0, p1, p2);
  }

  // load astrocyte into a tree
  qDebug() << QString(my_cell.name.c_str()) << " has: " << my_cell.triangles.size() << " Triangles";

  qDebug() << "Started building distance tree ...";
  tree tree(my_cell.triangles.begin(), my_cell.triangles.end());
  tree.accelerate_distance_queries();
  qDebug() << "Finished building distance tree ...";

  const auto mito_indices = mito->get_indices_list();

  double max_distance = 0.0;

  // compute distances for each mitochondrion vertex
  for (int idx : *mito_indices)
  {
    // dereferencing pointer
    auto vertex = &vertices->at(idx);
    point point_query(vertex->x(), vertex->y(), vertex->z());
    const auto distance = std::sqrt(tree.squared_distance(point_query));
    if (distance > max_distance)
      max_distance = distance;
    vertex->distance_to_cell = distance;
  }

  return EXIT_SUCCESS;
}
