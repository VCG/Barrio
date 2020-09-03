#include "mesh.h"
#include <fstream>
#include <sstream>

Mesh::Mesh()
{
  m_vertices.reserve(9000000);		   //v: 8,123,031
  m_faces.reserve(16500000);			   //f: 16,253,222
  m_typeVertexList[1].reserve(300000);   //mitos.v: 270,303
  m_typeVertexList[2].reserve(2900000);  //axons.v: 2,858,984
  m_typeVertexList[3].reserve(1400000);  //bouts.v: 1,300,846
  m_typeVertexList[4].reserve(14500000); //dends.v: 14,449,558
  m_typeVertexList[5].reserve(750000);   //spine.v: 696,496
  m_typeVertexList[6].reserve(1300000);  //astro.v: 1,246,096
  m_typeVertexList[7].reserve(350000);   //synps.v: 300,748
  m_typeVertexList[8].reserve(10000);
  m_typeVertexList[9].reserve(10000);
  m_typeVertexList[10].reserve(10000);
}


void Mesh::dumpObjects(QString path)
{
}

void Mesh::readObjects(QString path)
{
}

void Mesh::dumpVertices(QString path)
{
  std::string temp_string = path.toStdString();
  const char* filename = temp_string.c_str();

  std::ofstream outfile(filename, std::ios::binary);
  // write size of file into binary and read it the first thing to allocate
  int size = m_vertices.size(); // astro: 1246096
  outfile.write((char*)&size, sizeof(int));
  for (int i = 0; i < size; ++i) {
    outfile.write((char*)&m_vertices[i], sizeof(struct VertexData));
  }

  // write size of file into binary and read it the first thing to allocate
  size = m_faces.size(); // astro: 2497612
  outfile.write((char*)&size, sizeof(int));
  for (int i = 0; i < size; ++i) {
    outfile.write((char*)&m_faces[i], sizeof(struct Face));
  }

  outfile.close();
}

void Mesh::readVertices(QString path)
{
  std::string temp_string = path.toStdString();
  const char* filename = temp_string.c_str();

  int size = 0;
  std::ifstream ss(filename, std::ios::binary);

  if (ss.is_open()) {
    ss.read((char*)&size, sizeof(int)); // astro: 1246096
    // to make it dynamic I need to remove qvector4f and use float [4] instead -> not worth it
    for (int i = 0; i < size; i++) {
      struct VertexData v;
      ss.read((char*)&v, sizeof(struct VertexData));
      m_vertices.push_back(v);
    }

    size = 0;

    ss.read((char*)&size, sizeof(int));  // astro: 2497612
    // to make it dynamic I need to remove qvector4f and use float [4] instead -> not worth it
    for (int i = 0; i < size; i++) {
      struct Face f;
      ss.read((char*)&f, sizeof(struct Face));
      m_faces.push_back(f);
    }

    ss.close();
  }
  else {
    qDebug() << "Mesh::readVertexData: Problem in input file.";
  }

  qDebug() << " verticesList: " << m_vertices.size() << " m_faces: " << m_faces.size();
}

void Mesh::dumpMesh(QString path)
{
  dumpVertices(path + QString("/vertices.dat"));
  dumpNormals(path + QString("/normals.dat"));
}

void Mesh::readMesh(QString path)
{
  readVertices(path + QString("/vertices.dat"));
  readNormals(path + QString("/normals.dat"));
}

void Mesh::computeNormalsPerVertex()
{
  qDebug() << "Start Normals Computing Per vertex";

  for (int i = 0; i < m_vertices.size(); ++i) {
    struct VertexData vertex = m_vertices[i];
    QVector3D normal = QVector3D(0, 0, 0);

    // iterate over vertex faces
    std::vector<int>  faces_list = m_vertexFaces[i];
    for (int i = 0; i < faces_list.size(); ++i) {
      int f_idx = faces_list[i];
      struct Face f = m_faces[f_idx];
      QVector3D face_normal = this->computeFaceNormal(f);
      normal += face_normal;
    }

    normal.normalize();
    m_normals.push_back(normal.toVector4D());
  }
}

QVector3D Mesh::computeFaceNormal(struct Face f)
{
  QVector3D face_normal = QVector3D(0, 0, 0);
  if (f.v[0] >= m_vertices.size() || f.v[1] >= m_vertices.size() || f.v[2] >= m_vertices.size())
  {
    qDebug() << "^^^^^^^^^^^^^^ Problem this should not happen!";
    return face_normal;
  }

  struct VertexData v1 = m_vertices[f.v[0]];
  struct VertexData v2 = m_vertices[f.v[1]];
  struct VertexData v3 = m_vertices[f.v[2]];

  QVector3D A = v1.mesh_vertex.toVector3D();
  QVector3D B = v2.mesh_vertex.toVector3D();
  QVector3D C = v3.mesh_vertex.toVector3D();

  QVector3D AB = B - A;
  QVector3D AC = C - A;

  face_normal = QVector3D::crossProduct(AB, AC);

  float sin_alpha = face_normal.length() / (AB.length() * AC.length());
  return face_normal.normalized() * sin_alpha;
}

void Mesh::dumpNormals(QString path)
{
  std::string temp_string = path.toStdString();
  const char* filename = temp_string.c_str();

  std::ofstream outfile(filename, std::ios::binary);
  // write size of file into binary and read it the first thing to allocate
  int size = m_normals.size();
  outfile.write((char*)&size, sizeof(int));
  for (int i = 0; i < size; ++i) {
    outfile.write((char*)&m_normals[i], sizeof(QVector4D));
  }

  outfile.close();

}

bool Mesh::readNormals(QString path)
{
  std::string temp_string = path.toStdString();
  const char* filename = temp_string.c_str();

  int size = 0;
  std::ifstream ss(filename, std::ios::binary);
  if (ss.is_open()) {
    ss.read((char*)&size, sizeof(int));
    for (int i = 0; i < size; ++i) {
      QVector4D v;
      ss.read((char*)&v, sizeof(QVector4D));
      m_normals.push_back(v);
    }
  }
  else {
    return false;
  }


  return true;
}


int Mesh::addVertex(struct VertexData* vdata, Object_t type)
{
  int type_idx = static_cast<int>(type);
  m_typeVertexList[type_idx].push_back(m_vertices.data() + vdata->index);
  return vdata->index;
}

int Mesh::addVertexNormal(QVector4D vnormal)
{
  int idx = m_normals.size();
  m_normals.push_back(vnormal);
  return idx;
}

bool Mesh::isValidFaces(int f1, int f2, int f3)
{
  if (f1 > m_vertices.size()
    || f2 > m_vertices.size()
    || f3 > m_vertices.size()) {
    qDebug() << "Error in obj file, verticesList size: " << m_vertices.size();
    return false;
  }

  return true;
}

void Mesh::allocateVerticesVBO(QOpenGLBuffer vbo_mesh)
{
  vbo_mesh.allocate(m_vertices.data(), m_vertices.size() * sizeof(VertexData));
}

void Mesh::allocateNormalsVBO(QOpenGLBuffer vbo_mesh)
{
  vbo_mesh.allocate(m_normals.data(), m_normals.size() * sizeof(QVector4D));
}

void Mesh::addVertexNeighbor(int v_index, int face_index)
{
  m_vertexFaces[v_index].push_back(face_index);
}

void Mesh::addFace(int index1, int index2, int index3)
{
  struct Face f;
  f.v[0] = index1;
  f.v[1] = index2;
  f.v[2] = index3;

  int face_index = m_faces.size();
  m_faces.push_back(f);

  addVertexNeighbor(index1, face_index);
  addVertexNeighbor(index2, face_index);
  addVertexNeighbor(index3, face_index);
}

void Mesh::getVertexNeighbors(int v_index, std::set< int >& neighs)
{
  // Use 'mesh' to walk the connectivity.
  std::vector<int>  faces_list = m_vertexFaces[v_index];
  for (int i = 0; i < faces_list.size(); ++i) {
    struct Face f = m_faces[faces_list[i]];
    for (int j = 0; j < 3; ++j) {
      if (f.v[j] != v_index) {
        neighs.insert(f.v[j]);
      }
    }
  }
}

