#ifndef ABSTRACTIONSPACE_H
#define ABSTRACTIONSPACE_H

#include <vector>
#include <string>
#include <QVector4D>
#include "mainopengl.h"
#include "ssbo_structs.h"
#include "vismethod.h"

struct abstractionPoint {
  QVector2D point;
  int       ID;
};

struct VisConfiguration 
{
  bool axons;
  bool dends;
  bool mitos;
  bool syn;

  bool sliceView;
};

struct SelectedVisMethods 
{
  IVisMethod* low;
  IVisMethod* medium;
  IVisMethod* high;
};



class AbstractionSpace : public MainOpenGL
{
public:
  AbstractionSpace();
  ~AbstractionSpace();


  std::vector<struct abstractionPoint> get2DSpaceVertices() { return m_vertices; }
  std::vector<GLuint> get2DSpaceIndices() { return m_indices; }

  std::vector<struct abstractionPoint> get2DSpaceGridVertices() { return m_grid_vertices; }
  std::vector<GLuint> get2DSpaceGridIndices() { return m_grid_indices; }
  std::vector<struct abstractionPoint> get2DSpaceGridIlligalIndices() { return m_grid_illigal_vertices; }

  struct ast_neu_properties getSpaceProper() { return m_IntervalXY[m_intervalID]; }

  SelectedVisMethods configureVisMethods(VisConfiguration config);

private:
  struct pair_hash {
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1, T2>& p) const {
      auto h1 = std::hash<T1>{}(p.first);
      auto h2 = std::hash<T2>{}(p.second);
      return h1 ^ h2;
    }
  };

  int                                         m_xaxis;
  int                                         m_yaxis;

  int                                         m_intervalID;
  // OpenGL
  GLuint                                      m_buffer;
  GLuint                                      m_bindIdx;
  bool                                        m_glFunctionsSet;

  // Datas
  struct ssbo_2DState* m_2DState;

  std::vector< struct ast_neu_properties >    m_IntervalXY;

  std::vector<struct abstractionPoint>        m_vertices;
  std::vector<GLuint>                         m_indices;
  std::map<std::pair<int, int>, struct properties>
    m_neu_states;
  std::map<std::pair<int, int>, struct properties>
    m_ast_states;

  std::vector<struct abstractionPoint>        m_grid_vertices;
  std::vector<GLuint>                         m_grid_indices;
  std::vector<struct abstractionPoint>                         m_grid_illigal_vertices;

};

#endif // ABSTRACTIONSPACE_H
