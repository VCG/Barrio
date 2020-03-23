#include "mesh_preprocessing.h"

typedef CGAL::Simple_cartesian<double>              K;
typedef K::Point_3                                  Point;
typedef K::Triangle_3                               Triangle;
typedef std::list<Triangle>::iterator               Iterator;
typedef CGAL::AABB_triangle_primitive<K, Iterator>  Primitive;
typedef CGAL::AABB_traits<K, Primitive>             AABB_triangle_traits;
typedef CGAL::AABB_tree<AABB_triangle_traits>       Tree;

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

  std::vector<Point> objects;
  std::vector<Triangle> triangles;

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
        Point c = CGAL::centroid(triangles.begin(), triangles.end(), CGAL::Dimension_tag<2>());
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
      Point p(x, y, z);
      objects.push_back(p);
    }
    else if (line[0] == 'f') {
      int result = sscanf(line, "f %d//%d %d//%d %d//%d\n", &f1, &n1, &f2, &n2, &f3, &n3);
      triangles.push_back(Triangle(objects[f1], objects[f2], objects[f3]));
    }
  }

  Point c = CGAL::centroid(triangles.begin(), triangles.end(), CGAL::Dimension_tag<2>());
  QVector3D center(c.x(), c.y(), c.z());
  centers[hvgxID] = center;

  return EXIT_SUCCESS;
}

struct Astrocyte {
  std::string name;
  std::list<Triangle> triangles;
  std::vector<Point> vertices;
};

// Load astrocyte mesh and build its tree
// Load other objects (neurites)
// for each other object, check against the astrocyte, and get the closest vertices from neurites to skeleton mesh
// Goal: find all closest points to astrocyte

int MeshProcessing::computeDistance(QString astro_obj_path, std::vector<VertexData> &neurites_vertices)
{
  FILE* fp_astro = fopen(astro_obj_path.toStdString().c_str(), "r");

  if (!fp_astro) {
    std::cout << "error in file" << std::endl;
    return 0;
  }

  struct Astrocyte astro;
  double x, y, z;
  int f1, f2, f3;
  int n1, n2, n3;
  Point p;
  char line[1024];
  while (fgets(line, 1024, fp_astro)) {
    if (line[0] == 'o') { // read obj vertices
      astro.name = line;
    }
    else if (line[0] == 'v') {
      int result = sscanf(line, "%*s%lf%lf%lf", &x, &y, &z);
      Point p(x, y, z);
      astro.vertices.push_back(p);
    }
    else if (line[0] == 'f') {
      int result = sscanf(line, "f %d//%d %d//%d %d//%d\n", &f1, &n1, &f2, &n2, &f3, &n3);
      Point p1 = astro.vertices.at(f1 - 1);
      Point p2 = astro.vertices.at(f2 - 1);
      Point p3 = astro.vertices.at(f3 - 1);
      astro.triangles.push_back(Triangle(p1, p2, p3));
    }
  }

  // load astrocyte into a tree
  std::cout << "Astrocyte has: " << astro.triangles.size() << " Triangles" << std::endl;

  std::cout << "Started building tree ..." << std::endl;
  Tree tree(astro.triangles.begin(), astro.triangles.end());
  tree.accelerate_distance_queries(); // the astrocyte in a tree
  std::cout << "Finished building tree ..." << std::endl;

  // compute distances for each neurite vertex
  for (size_t i = 0; i < neurites_vertices.size(); i++)
  {
    VertexData vertex = neurites_vertices.at(i);
    Point point_query(vertex.x(), vertex.y(), vertex.z());
    double distance = std::sqrt(tree.squared_distance(point_query));
    std::cout << "Distance: " << distance << std::endl;
    vertex.skeleton_vertex.setW(distance);
    neurites_vertices[i] = vertex;
  }

  return EXIT_SUCCESS;
}
