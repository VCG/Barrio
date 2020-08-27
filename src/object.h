// todo: add mitochondira node to neurites skeletons
// neurites dont have points cooridnates
// they use parents nodes coordinates so they only have index in the skeleton
//
#ifndef OBJECT_H
#define OBJECT_H

#include <QVector3D>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

#include <cereal/archives/binary.hpp>
#include <cereal/access.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

#include "skeleton.h"
#include "mesh.h"
#include "ssbo_structs.h"

#define ASTRO_DIST_THRESH   0.0001

// if object type == synapse
struct synapse {
  int axon;
  int dendrite;
  int spine;
  int bouton;

  template<class Archive>
  void serialize(Archive& ar)
  {
    ar(axon, dendrite, spine, bouton);
  }
};

// type identifiers for objects
enum class Object_t { UNKNOWN = 0, MITO = 1, AXON = 2, BOUTON = 3, DENDRITE = 4, SPINE = 5, ASTROCYTE = 6,
  SYNAPSE = 7, GLYCOGEN = 8, ASTRO_MITO = 9 };

class Skeleton;
class Object
{
public:
  Object();
  Object(std::string name, int ID);
  ~Object();

  // temporary skeleton data points handleing function
  void add_s_vertex(struct SkeletonPoint vertex_data);

  int writeSkeletontoVBO(QOpenGLBuffer vbo, int offset);
  // mesh indices functions
  void addTriangleIndex(int faces);
  size_t get_indices_size() { return m_meshIndices.size(); }
  void* getIndexData() { return m_meshIndices.data(); }
  std::vector<int>* get_indices_list(){ return &m_meshIndices; }



  // properties getters
  bool hasParent();
  Object_t getObjectType();
  std::string getName() { return m_name; }
  QVector4D getColor();
  QVector4D getCenter();
  QVector4D getAstPoint() { return m_ast_point; }
  int getVolume() { return m_volume; }
  int getHVGXID() { return m_ID; }
  struct ssbo_mesh getSSBOData();

  //Object* getParent() { return m_parent; }
  int getParentID() { return m_parentID; }

  std::vector<Object*> getChildren() { return m_children; }
  float getAstroCoverage();
  int getFunction() { return m_function; }

  // properties setters
  void setFunction(int function) { m_function = function; }
  void setColor(QVector4D color) { m_color = color; }
  void setCenter(QVector4D center);
  void setAstPoint(QVector4D ast_point);
  void setVolume(int volume) { m_volume = volume; }

  void setDistanceToStructure(int structure_hvgx, double distance) { m_distance_map[structure_hvgx] = distance; };

  void setParentID(int parentID) { m_parentID = parentID; }
  //void setParent(Object* parent) { m_parent = parent; }

  void addChild(Object* child);
  void addChildID(int hvgxID);

  std::vector<int>* getChildrenIDs();

  // skeleton management
  void addSkeletonNode(QVector3D coords);
  void addSkeletonPoint(QVector3D coords);
  bool addSkeletonBranch(SkeletonBranch* branch, Object* parent);
  Skeleton* getSkeleton() { return m_skeleton; }
  void fixSkeleton(Object* parent);
  void setSkeletonOffset(int offset) { m_skeleton->setIndexOffset(offset); }
  int  getSkeletonOffset() { return m_skeleton->getIndexOffset(); }
  void setNodeIdx(int node_index) { m_nodeIdx = node_index;}
  int  getNodeIdx() { return m_nodeIdx; }

  void markChildSubSkeleton(SkeletonBranch* branch, int ID);

  void updateFilteredFlag(bool isFiltered) { m_isFiltered = isFiltered; }
  bool isFiltered() { return m_isFiltered; }


  void updateAstSynapseFlag(bool flag) { m_isAstroSynapse = flag; }

  void updateClosestAstroVertex(float dist, int vertexIdx);
  void UpdateSynapseData(int axons_id, int dendrite_id, int spine_id, int bouton_id);
  void addSynapse(Object* synapse_object);
  std::vector<Object*> getSynapses() { return m_synapses; }
  struct synapse getSynapseData() { return m_synapse_data; }

  void setMappedValue(float value) { m_mappedValue = value; }
  float getMappedValue() { return m_mappedValue; }

  int getSynapseSize();

  bool isChild(int hvgxID);
  bool isParent(int hvgxID);

  std::map<int, double>* get_distance_map_ptr() { return &m_distance_map; };
  std::vector<std::pair<int, double>>* get_distance_vector_ptr();

  
private:

  std::string                             m_name;
  int                                     m_ID;           // hvgx
  int                                     m_nodeIdx;

  Object_t                                m_object_t;     /* object type */
  int                                     m_volume;       // volume of this object
  int                                     m_function;     // -1:not applicable, 0:ex, 1:in, 3:unknown

  //Object*                                 m_parent;       // NULL if none
  int                                     m_parentID;
  std::vector<Object*>                    m_children;     // axon-> bouton, den->spine
  std::vector<int>                        m_children_ids;

  std::map<int, double>                   m_distance_map;
  std::vector<std::pair<int, double>>     m_distance_vector;

  QVector4D                               m_center;
  float                                   x_center, y_center, z_center, w_center;
  QVector4D                               m_ast_point;    // closest point from astrocyte skeleton to this object so we can project the object on skeleton and be part of it
  QVector4D                               m_color;

  Skeleton*                               m_skeleton;

  std::vector<int>                        m_meshIndices;  // indices to access the global mesh vertices defined in meshv

  bool                                    m_isFiltered;   // 1 yes, 0 no
  bool                                    m_isAstroSynapse;

  std::pair<int, float>                   m_closest_astro_vertex;
  std::vector<int>                        m_VertexidxCloseToAstro;  // less than ASTRO_DIST_THRESH ----> we can refine this later
  float                                   m_averageDistance;
  float									  m_mappedValue;

  
  std::vector<Object*>                    m_synapses;     // list of synapses
  std::vector<int>                        m_synapse_ids;
  struct synapse                          m_synapse_data;

  //std::map<Object_t, void*>               m_dataByType;   // use this to store info based on data type

  friend class cereal::access;

  template<class Archive>
  void serialize(Archive& ar)
  {
    ar(m_name, m_ID, m_nodeIdx,
      m_object_t, m_volume, m_function,
      m_parentID, m_children_ids,
      m_meshIndices, 
      m_isFiltered, m_isAstroSynapse,
      m_VertexidxCloseToAstro, m_averageDistance, m_mappedValue,
      m_synapse_ids, m_synapse_data, x_center, y_center, z_center, w_center, m_distance_map);


  }
};

#endif // OBJECT_H
