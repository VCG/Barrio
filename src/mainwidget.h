#pragma once
#include <QOpenGLWidget>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <filesystem>

#include "glwidget.h"
#include "datacontainer.h"

enum NumberOfEntities
{
  LOW, MEDIUM, HIGH
};

class MainWidget: public QOpenGLWidget, protected QOpenGLFunctions
{
  Q_OBJECT;
public:
  MainWidget(DataContainer* datacontainer, InputForm* input_form, QWidget* parent = 0);
  bool addWidgetGroup(int ID, bool isOverviewWidget);
  bool deleteInfoVisWidget(int ID);
  bool deleteAllInfoVisWidgets();

  bool deleteAllGLWidgets();

  bool addInfoVisWidget(int ID, QString name, IVisMethod* visMethod);
  bool addGLWidget(int ID, QString name, bool isOverviewWidget);

  void setupMainWidget(VisConfiguration vis_config);
  SelectedVisMethods setThumbnailIcons(VisConfiguration vis_config);

  void updateInfoVisViews();
  void setNumberOfEntities(NumberOfEntities number_of_entities);
  

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

  std::map<int, QGroupBox*> m_gl_boxes;
  std::map<int, GLWidget*> m_GL_widgets;
  
  std::map<int, QGroupBox*> m_info_vis_boxes;
  std::map<int, IVisMethod*> m_views;

  QList<int>    getSelectedIDs();

  QGridLayout* m_gl_layout;

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
  NumberOfEntities m_number_of_entities;

  QString m_low_detail_name = "Low Detail";
  QString m_medium_detail_name = "Medium Detail";
  QString m_high_detail_name = "High Detail";

  

};

