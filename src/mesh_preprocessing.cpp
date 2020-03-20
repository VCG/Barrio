#include "mesh_preprocessing.h"



typedef double                      FT;
typedef CGAL::Simple_cartesian<FT>  K;
typedef K::Point_2                  Point_2;
typedef K::Point_3                  Point_3;
typedef K::Triangle_3               Triangle_3;

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
  std::ofstream outfile;
  std::string outname = "../../processed_data/mouse03_centroids.txt";
  std::cout << outname << std::endl;
  outfile.open(outname);

  FILE* fp = fopen(obj_path.toStdString().c_str(), "r");
  if (!fp) return 0;
  char line[1024];

  std::vector<Point_3> objects;
  std::vector<Triangle_3> triangles_3;

  double x, y, z;
  int f1, f2, f3;
  std::string name;
  int flag = 0;
  int i = 0;
  // make this a function: fp, &objects
  while (fgets(line, 1024, fp)) {
    // read obj vertices
    if (line[0] == 'o') {
      std::istringstream ss(line);
      std::string item;
      getline(ss, item, ',');
      if (flag == 1 && triangles_3.size() > 1) {
        Point_3 c3 = CGAL::centroid(triangles_3.begin(), triangles_3.end(), CGAL::Dimension_tag<2>());
        std::cout << name;
        std::cout << c3 << std::endl;
        outfile << name;
        outfile << c3 << std::endl;
      }

      name = line;

      triangles_3.clear();
      flag = 1;
    }
    else if (line[0] == 'v') {
      sscanf(line, "%*s%lf%lf%lf", &x, &y, &z);
      Point_3 p(x, y, z);
      objects.push_back(p);
    }
    else if (line[0] == 'f') {
      sscanf(line, "%*s%d%d%d", &f1, &f2, &f3);
      Point_3 p1 = objects[f1];
      Point_3 p2 = objects[f2];
      Point_3 p3 = objects[f3];
      triangles_3.push_back(Triangle_3(p1, p2, p3));
    }
  }

  Point_3 c3 = CGAL::centroid(triangles_3.begin(), triangles_3.end(), CGAL::Dimension_tag<2>());
  std::cout << name;
  std::cout << c3 << std::endl;
  outfile << name;
  outfile << c3 << std::endl;
  outfile.close();

  return 0;
}

typedef CGAL::Simple_cartesian<double> K;
typedef K::FT FT;
typedef K::Point_3 Point;
typedef K::Triangle_3 Triangle;
typedef std::list<Triangle>::iterator Iterator;
typedef CGAL::AABB_triangle_primitive<K, Iterator> Primitive;
typedef CGAL::AABB_traits<K, Primitive> AABB_triangle_traits;
typedef CGAL::AABB_tree<AABB_triangle_traits> Tree;
typedef Tree::Point_and_primitive_id Point_and_primitive_id;


struct obj {
  std::string name;
  std::list<Triangle> triangles;
  std::vector<Point> vertices;
};

// Load astrocyte mesh and build its tree
// Load other objects (neurites)
// for each other object, check against the astrocyte, and get the closest vertices from neurites to skeleton mesh
// Goal: find all closest points to astrocyte

int MeshProcessing::computeDistance(QString astro_obj_path, QString neurites_obj_path)
{
  const char* filename_astro = astro_obj_path.toStdString().c_str();
  const char* filename_neu = neurites_obj_path.toStdString().c_str();
  std::ofstream out("C:\\Users\\jtroidl\\Desktop\\experimental_data\\Mouse_3\\m3_neurites_AstDist.obj.bin", std::ios::binary);

  FILE* fp_astro = fopen(filename_astro, "r");

  if (!fp_astro) {
    std::cout << "error in file" << std::endl;
    return 0;
  }

  struct obj astro;
  //std::vector<Point> vertices;
  double x, y, z;
  int f1, f2, f3;
  Point p;
  char line[1024];
  while (fgets(line, 1024, fp_astro)) {
    // read obj vertices
    if (line[0] == 'o') {
      astro.name = line;
    }
    else if (line[0] == 'v') {
      sscanf(line, "%*s%lf%lf%lf", &x, &y, &z);
      Point p(x, y, z);
      astro.vertices.push_back(p);
      // insert them in triangles
    }
    else if (line[0] == 'f') {
      sscanf(line, "%*s%i%i%i", &f1, &f2, &f3);
      Point p1 = astro.vertices.at(f1 - 1);
      Point p2 = astro.vertices.at(f2 - 1);
      Point p3 = astro.vertices.at(f3 - 1);
      astro.triangles.push_back(Triangle(p1, p2, p3));
    }
  }

  // load astrocyte into a tree
  std::cout << "Astrocyte has: " << astro.triangles.size() << " Triangles" << std::endl;

  Tree tree(astro.triangles.begin(), astro.triangles.end());
  tree.accelerate_distance_queries(); // the astrocyte in a tree

  int vertex_index = 0;
  std::ifstream infile(filename_neu);
  for (std::string sline; std::getline(infile, sline); ) {
    std::istringstream ss(sline);
    std::vector<std::string> record;
    std::string item;
    while (std::getline(ss, item, ' ')) {
      record.push_back(item);
    }

    // read obj vertices
    if (record[0][0] == 'o') {
      std::cout << '\n' << sline << std::endl;
      sline += '\n';
      out.write(sline.c_str(), sline.size());

      // WRITE THIS LINE TO FILE
    }
    else if (record[0][0] == 'v') {
      ++vertex_index;
      x = atof(record[1].c_str());
      y = atof(record[2].c_str());
      z = atof(record[3].c_str());
      Point point_query(x, y, z);
      Point_and_primitive_id pp = tree.closest_point_and_primitive(point_query);
      FT sqd = tree.squared_distance(point_query);
      std::ostringstream X, Y, Z, DIST;
      X << x;
      Y << y;
      Z << z;
      DIST << std::sqrt(sqd);
      std::string newline = "v " + X.str() + " " + Y.str() + " "
        + Z.str() + " " + DIST.str() + "\n";
      long size = newline.size();
      out.write(newline.c_str(), size);
    }
    else {
      sline += '\n';
      out.write(sline.c_str(), sline.size());
    }
  }


  return EXIT_SUCCESS;
}
