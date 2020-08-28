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

struct MyStructure {
  std::string name;
  std::list<triangle> triangles;
  std::vector<point> vertices;
};

int MeshProcessing::computeCenter(Object* obj, std::vector<VertexData>* vertices)
{
  struct MyStructure my_structure;
  my_structure.name = obj->getName();

  std::vector<int>* indices = obj->get_indices_list();

  // extract vertex data
  for (auto i = 0; i < indices->size(); i = i + 3)
  {
    // vertex 0
    const int idx_0 = (*indices)[i]; // dereferencing pointer
    auto v0 = vertices->at(idx_0);
    point p0(v0.x(), v0.y(), v0.z());
    my_structure.vertices.push_back(p0);

    // vertex 1
    const int idx_1 = (*indices)[i + 1]; // dereferencing pointer
    auto v1 = vertices->at(idx_1);
    point p1(v1.x(), v1.y(), v1.z());
    my_structure.vertices.push_back(p1);

    // vertex 2
    const int idx_2 = (*indices)[i + 2]; // dereferencing pointer
    auto v2 = vertices->at(idx_2);
    point p2(v2.x(), v2.y(), v2.z());
    my_structure.vertices.push_back(p2);

    // add face
    my_structure.triangles.emplace_back(p0, p1, p2);
  }

  point c = CGAL::centroid(my_structure.triangles.begin(), my_structure.triangles.end(), CGAL::Dimension_tag<2>());
  QVector4D center(c.x(), c.y(), c.z(), 1.0);
  obj->setCenter(center);

  return EXIT_SUCCESS;
}



// Load astrocyte mesh and build its tree
// Load other objects (neurites)
// for each other object, check against the astrocyte, and get the closest vertices from neurites to skeleton mesh
// Goal: find all closest points to astrocyte

int MeshProcessing::compute_distance_distribution(Object* mito, Object* cell, std::vector<VertexData>* vertices) const
{
  struct MyStructure my_cell;
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
    auto vertex = &vertices->at(idx);
    point point_query(vertex->x(), vertex->y(), vertex->z());
    const auto distance = std::sqrt(tree.squared_distance(point_query));
    if (distance > max_distance)
      max_distance = distance;
    vertex->distance_to_cell = distance;
  }

  return EXIT_SUCCESS;
}

double MeshProcessing::compute_closest_distance(Object* from, Object* to, std::vector<VertexData>* vertices)
{
  struct MyStructure structure;
  structure.name = to->getName();

  std::vector<int>* cell_indices = to->get_indices_list();

  // extract vertex data
  for (auto i = 0; i < cell_indices->size(); i = i + 3)
  {
    // vertex 0
    const int idx_0 = (*cell_indices)[i]; // dereferencing pointer
    auto v0 = vertices->at(idx_0);
    point p0(v0.x(), v0.y(), v0.z());
    structure.vertices.push_back(p0);

    // vertex 1
    const int idx_1 = (*cell_indices)[i + 1]; // dereferencing pointer
    auto v1 = vertices->at(idx_1);
    point p1(v1.x(), v1.y(), v1.z());
    structure.vertices.push_back(p1);

    // vertex 2
    const int idx_2 = (*cell_indices)[i + 2]; // dereferencing pointer
    auto v2 = vertices->at(idx_2);
    point p2(v2.x(), v2.y(), v2.z());
    structure.vertices.push_back(p2);

    // add face
    structure.triangles.emplace_back(p0, p1, p2);
  }

  // load astrocyte into a tree
  qDebug() << QString(structure.name.c_str()) << " has: " << structure.triangles.size() << " Triangles";

  qDebug() << "Started building distance tree ...";
  tree tree(structure.triangles.begin(), structure.triangles.end());
  tree.accelerate_distance_queries();
  qDebug() << "Finished building distance tree ...";

  const auto from_indices = from->get_indices_list();

  double min_distance = std::numeric_limits<double>::max();

  // compute distances for each mitochondrion vertex
  for (int idx : *from_indices)
  {
    // dereferencing pointer
    auto vertex = &vertices->at(idx);
    point point_query(vertex->x(), vertex->y(), vertex->z());
    const auto distance = std::sqrt(tree.squared_distance(point_query));
    if (distance < min_distance)
      min_distance = distance;
  }

  return min_distance;
}
