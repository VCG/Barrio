#ifndef MESH_H
#define MESH_H

#include "mainopengl.h"

#include <set>
#include <vector>

#include <cereal\archives\binary.hpp>
#include <cereal\types\map.hpp>
#include <cereal\types\vector.hpp>




enum class Object_t;

struct Face {
  int v[3];
};

// mesh vertex
struct VertexData {
  QVector4D   mesh_vertex;
  float       distance_to_cell;
  GLint         structure_type;
  GLint         hvgxID;

  QVector4D   skeleton_vertex;
  int		  index;

  float  x()  const
  {
    return mesh_vertex.x();
  }
  float  y()  const
  {
    return mesh_vertex.y();
  }
  float  z()  const
  {
    return mesh_vertex.z();
  }
  float id() const
  {
    return mesh_vertex.w();
  }
};


/*Stores all vertices and faces of one mouse dataset*/
class Mesh
{
public:
  Mesh();

  int addVertex(struct VertexData* vdata, Object_t type);
  int addVertexNormal(QVector4D vnormal);
  bool isValidFaces(int f1, int f2, int f3);
  size_t  getVerticesSize() { return m_vertices.size(); }

  void addVertexNeighbor(int v_index, int face_index);

  std::vector< struct VertexData >* getVerticesList() { return &m_vertices; }
  std::vector< VertexData* >* getVerticesListByType(Object_t type) { return &(m_typeVertexList[(int)type]); }
  std::vector<QVector4D>* getNormalsList() { return &m_normals; }

  void addFace(int index1, int index2, int index3);
  void getVertexNeighbors(int v_index, std::set< int >& neighs);

  std::vector< struct Face >* getFacesList() { return &m_faces; }
  size_t getFacesListSize() { return m_faces.size(); }

  // opengl functions
  void allocateVerticesVBO(QOpenGLBuffer vbo_mesh);
  void allocateNormalsVBO(QOpenGLBuffer vbo_mesh);

  void dumpVertices(QString path);
  void readVertices(QString path);

  void dumpNormals(QString path);
  bool readNormals(QString path);

  void dumpObjects(QString path);
  void readObjects(QString path);

  void dumpMesh(QString path);
  void readMesh(QString path);

  void computeNormalsPerVertex();
  QVector3D computeFaceNormal(struct Face);

  

  size_t getNormalsListSize() { return m_normals.size(); }

protected:
  
  // each vertex could belong to more than one face
  // for each vertex in each face, add face to vertex

  std::vector< struct Face >            m_faces; // sequential, write
  std::vector< struct VertexData >      m_vertices;
  std::vector< QVector4D >              m_normals;

  std::map<int, std::vector<int> >      m_vertexFaces;
  std::vector<VertexData*>              m_typeVertexList[11];

 
};

#endif // MESH_H


