// cluster glycogen
// color glycoen based on cluster
// find closest object to center of cluster
// color that object with glycogen cluster

// todo: normalize volume baed on the non filtered objects
// get the largest volume from all present objects
// and normalize based on that
// so whwn dendrites is filtered, other volumes can be relatively objserved without astrocyte

#ifndef OBJECTMANAGER_H
#define OBJECTMANAGER_H

// file manipulations
#include <QString>
#include <QFile>

#include <QXmlStreamReader>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QDebug>
#include <string>

#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/map.hpp>

#include "object.h"
#include "mesh.h"
#include "grid3d.h"
#include "spatialhash3d.h"
#include "inputform.h"

#include "globalParameters.h"
#include "mesh_preprocessing.h"

struct MyEdge {
  int p1;
  int p2;
};

struct MyBranch {

  QVector<int> indices;
  int start_index;
  int end_index;
  
  double length;
  double avg_radius;
  double min_radius;
  double max_radius;

  double vertex_count;

  int type; // bouton/spine or cell body
};

struct MySkeleton {
  int hvgx;
  QMap<int, MyBranch> branches;
};

class DataContainer
{
public:
  DataContainer(InputForm* input_form);
  ~DataContainer();

  void loadData();

  bool isNormalsEnabled();

  void parseSkeleton(QXmlStreamReader& xml, Object* obj);
  void parseSkeletonNodes(QXmlStreamReader& xml, Object* obj);
  void parseBranch(QXmlStreamReader& xml, Object* obj);
  void parseSkeletonPoints(QXmlStreamReader& xml, Object* obj);

  int getObjectThatHasSynapse(int syn_id, int not_this_object);
  QList<int> getMitosOfCell(int cell_id);

  bool importObj(QString path);
  bool importSkeletons(QString path, Object_t skeleton_type);
  bool importSemanticSkeleton(QString path);

  MySkeleton processSkeletonStructure(int hvgx, QList<int>* edges, std::vector< struct VertexData >* vertices);

  void processParentChildStructure();

  void loadConnectivityGraph(QString path);
  void addEdgeToConnectivityGraph(int, int, std::set< std::tuple<int, int> >&);
  void parseSynapsesGraph(QList<QByteArray>&, std::set< std::tuple<int, int> >&);
  void PostloadMetaDataHVGX(QString path);
  void PreLoadMetaDataHVGX(QString path);
  bool findAndSetParentID(Object* obj, int hvgxID);

  bool hasSemanticSkeleton(int hvgx);

  void dumpObjects(QString path);
  void readObjects(QString path);

  std::vector<VertexData*>*           getGlycogenVertexDataPtr() { return &m_glycogenList; }

  SpacePartitioning::SpatialHash3D*   getSpineHash() { return &m_spineHash; }
  SpacePartitioning::SpatialHash3D*   getBoutonHash() { return &m_boutonHash; }
  SpacePartitioning::SpatialHash3D*   getNeuroMitoHash() { return &m_neuroMitoHash; }
  unsigned char*                      getGlycogen3DGridData();
  void								  resetMappingValues();


  // graph related function
  std::map<int, Object*>  getObjectsMap();
  std::map<int, Object*>* getObjectsMapPtr() { return &m_objects; }
  std::vector<QVector2D>  getNeuritesEdges();

  QVector<int>*           getSkeletonIndices() { return &m_skeleton_indices; }

  int getIndexByName(QString name);

  void writeDataToCache(QString cache_path);
  void loadDataFromCache(QString cache_path);
  void compute_distance_mito_cell_boundary();
  void compute_centers();

  void compute_synapse_distances(Object* mito); // computes distance to closest synapse for the given mitochondrion
  void compute_mito_distances(Object* synapse); // computes distance to closest mitochondrion for the given synapse
  void compute_closest_distance_to_structures();


  int   getSkeletonPointsSize();
  int   getMeshIndicesSize();
  Mesh* getMesh();

  Object_t              getObjectTypeByID(int hvgxID);
  std::string           getObjectName(int hvgxID);
  std::vector<Object*>  getObjectsByType(Object_t type);
  Object*               getObject(int hvgxID);
  Object*               getObjectByName(QString name);

  bool					isWithinDistance(int from, int to, float distance);

  void                  addSliceVertices();
  int                   addSliceVertex(float x, float y, float u, float v);

  float   getMaxAstroCoverage() { return max_astro_coverage; }
  int     getMaxVolume() { return max_volume; }
  int     getMaxSynapseVolume() { return max_synapse_volume; }

  // iterate over objects and get max volume and astro coverage
  void recomputeMaxValues(bool weighted);

  //************ Load Raw Data
  char* loadRawFile(QString path, int size);


  void buildMissingSkeletons();

  int getIndicesSizeByObjectType(Object_t type) { return m_indices_size_byType[type]; }
  int getTypeByID(int hvgx); 

public:
  struct input_files                          input_files_dir;
  QString                                     m_sematic_skeleton_json;

protected:
  // maximum volume from displayed objects
  int                                         max_volume;
  float                                       max_synapse_volume;
  // maximum vertices from neurites covered by astrocyte
  float                                       max_astro_coverage;

  // store all vertices of the mesh.
  // unique vertices, faces to index them.
  Mesh* m_mesh;

  Object* m_curParent;
  int                                         m_skeleton_points_size;
  int                                         m_indices_size; // used to allocate indices of a mesh
  int                                         m_vertex_offset; // used to unify vertices for one mesh
  int                                         m_faces_offset; // used to unify vertices for one mesh

  int                                         m_limit;

  // objects
  std::map<int, int>                          m_parents;
  std::map<int, Object*>                      m_objects;
  std::map<Object_t, std::vector<Object*> >   m_objectsByType;
  std::map<Object_t, int >                    m_indices_size_byType;
  std::map<std::string, int>                  m_id_name_map;

  QVector<int>                                m_skeleton_indices;


  // graph related data
  std::vector<QVector2D>                      neurites_neurite_edge;

  // glycogen
  std::vector<VertexData*>                    m_glycogenList;
  SpacePartitioning::Grid3D					  m_glycogen3DGrid;

  SpacePartitioning::SpatialHash3D			  m_boutonHash;
  SpacePartitioning::SpatialHash3D			  m_spineHash;
  SpacePartitioning::SpatialHash3D			   m_neuroMitoHash;

  // file management
  Normals_t                                   m_normals_t;
  LoadFile_t                                  m_loadType;
  LoadData_t                                  m_load_data;
  bool                                        m_debug_msg;
  std::set<Object*>                           m_missingParentSkeleton;

 private:
   std::map<int, Object>                     serializable_objects;
   QString                                   objs_filename;

   MeshProcessing*                            m_mesh_processing;

   bool edgeTouchesForkVertex(MyEdge edge, QVector<int>* fork_vertices);
   bool edgeContainsIndex(MyEdge edge, int index);

   void display(const std::vector<MyBranch>* branches, int n);
   void findPermutations(QMap<int, MyBranch>* branches);
};

#endif // OBJECTMANAGER_H
