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
  QVector4D   mesh_vertex;        // w: Object ID (hvgx ID)
  QVector4D   skeleton_vertex;    // w: markers distance to astrocyte (since this is per vertex, compute the distance from each vertex to astrocyte)

  
  int			index;
  //bool		isGlycogen;			//because glycogen ids and object ids are seperate

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

  void addFace(int index1, int index2, int index3);
  void getVertexNeighbors(int v_index, std::set< int >& neighs);

  std::vector< struct Face >* getFacesList() { return &m_faces; }
  size_t getFacesListSize() { return m_faces.size(); }

  // opengl functions
  void allocateVerticesVBO(QOpenGLBuffer vbo_mesh);
  void allocateNormalsVBO(QOpenGLBuffer vbo_mesh);

  void dumpVertexData(QString path);
  void readVertexData(QString path);

  void dumpMesh();
  void readMesh();

  void computeNormalsPerVertex();
  QVector3D computeFaceNormal(struct Face);

  void dumpNormalsList(QString path);
  bool readNormalsBinary(QString path);

  size_t getNormalsListSize() { return m_normals.size(); }

protected:
  
  // each vertex could belong to more than one face
  // for each vertex in each face, add face to vertex

  std::vector< struct Face >            m_faces; // sequential, write
  std::vector<struct VertexData >       m_vertices;
  std::vector< QVector4D >              m_normals;

  std::map<int, std::vector<int> >      m_vertexFaces;
  std::vector<VertexData*>              m_typeVertexList[9];

 
};

#endif // MESH_H


