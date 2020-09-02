#pragma once

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <filesystem>

#include "mainopengl.h"
#include "glwidget.h"
#include "datacontainer.h"
#include "stb_image.h"

enum NumberOfEntities
{
  LOW, MEDIUM, HIGH
};

class MainWidget: public QOpenGLWidget, MainOpenGL
{
  Q_OBJECT;
public:
  MainWidget(DataContainer* datacontainer, InputForm* input_form, QWidget* parent = 0);
  
  bool addWidgetGroup(int ID, bool isOverviewWidget);
  //bool deleteInfoVisWidget(int ID);
  
  bool deleteAllInfoVisWidgets();
  bool deleteAllGLWidgets();
  bool deleteAllWidgets(bool deleteGeneralInfoVisWidgets);

  void addCloseButtonToWidget(QGroupBox* groupBox);

  bool addInfoVisWidget(int ID, QGroupBox* groupBox, IVisMethod* visMethod);
  bool addGLWidget(int ID, QGroupBox* groupBox, bool isOverviewWidget);

  void setupMainWidget(VisConfiguration vis_config);
  SelectedVisMethods setThumbnailIcons(VisConfiguration vis_config);

  void updateInfoVisViews();
  void setNumberOfEntities(NumberOfEntities number_of_entities);

  double on_synapse_distance_slider_changed(int value);
  void on_opacity_slider_changed(int value);

  //bool initOpenGLFunctions();

  void initializeGL() override;
  void paintGL() override;
  void resizeGL(int width, int height) override;
  //void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;
  

public slots:
  void on_widget_close_button_clicked();

private:
  DataContainer* m_datacontainer;
  InputForm* m_input_form; // bad

  std::map<int, QGroupBox*>   m_groupboxes;
  std::map<int, GLWidget*>    m_opengl_views;
  std::map<int, IVisMethod*>  m_infovis_views;
  std::map<int, QFrame*>      m_seperation_elements;

  QList<int>    getSelectedIDs();

  QGridLayout* m_main_layout;

  SharedGLResources m_shared_resources;

  // shared resource variables
  QOpenGLBuffer m_mesh_index_vbo;
  QOpenGLBuffer m_mesh_vertex_vbo;
  QOpenGLBuffer m_mesh_normal_vbo;

  GLuint        m_mito_cell_distance_colormap;
  GLuint        m_image_stack_texture;

  QOpenGLBuffer m_slice_vertex_vbo;
  GLuint        m_slice_texture;

  AbstractionSpace* m_abstraction_space;

  SelectedVisMethods m_vis_methods;

  void initSharedVBOs();
  void initSharedSlice();
  void initSharedMeshVBOs();
  void initColormaps();
  void init3DVolumeTexture();

  void init_1D_texture(GLuint& texture, GLenum texture_unit, GLvoid* data, int size);
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

