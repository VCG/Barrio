#pragma once
#include <QOpenGLWidget>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <filesystem>

#include "glwidget.h"
#include "datacontainer.h"


class MainWidget: public QOpenGLWidget, protected QOpenGLFunctions
{
  Q_OBJECT;
public:
  MainWidget(DataContainer* datacontainer, InputForm* input_form, QWidget* parent = 0);
  bool addWidgetGroup(int ID, bool isOverviewWidget);
  bool deleteWidget(int ID);

  bool addInfoVisWidget(int ID, QString name);
  bool addGLWidget(int ID, QString name, bool isOverviewWidget);

  void setupMainWidget(VisConfiguration vis_config);

public slots:
  void on_synapse_distance_slider_changed(int value);

protected:
  void initializeGL() override;
  void paintGL() override;
  void resizeGL(int width, int height) override;
  void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;

private:
  DataContainer* m_datacontainer;
  InputForm* m_input_form; // bad
  std::map<int, GLWidget*> m_widgets;
  QGridLayout* m_layout;
  SharedGLResources m_shared_resources;

  // shared resource variables
  QOpenGLBuffer m_mesh_index_vbo;
  QOpenGLBuffer m_mesh_vertex_vbo;
  QOpenGLBuffer m_mesh_normal_vbo;

  QOpenGLBuffer m_slice_vertex_vbo;
  GLuint        m_slice_texture;

  AbstractionSpace* m_abstraction_space;

  SelectedVisMethods m_vis_methods;

  void initSharedVBOs();
  void initSharedSliceVBOs();
  void initSharedMeshVBOs();

  void load3DTexturesFromRaw(QString path, GLuint& texture, GLenum texture_unit, int sizeX, int sizeY, int sizeZ);

  int m_lastID;
  int m_current_row = 0;
  int m_current_col = 0;


  int m_max_cols = 3;

  int m_number_of_selected_structures = 0;

};

