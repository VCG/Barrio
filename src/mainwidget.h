#pragma once
#include <QOpenGLWidget>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "glwidget.h"
#include "datacontainer.h"

class MainWidget: public QOpenGLWidget, protected QOpenGLFunctions
{
public:
  MainWidget(InputForm* input_form, QWidget* parent = 0);
  bool addGLWidget(int ID, int row, int col);
  bool deleteWidget(int ID);

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

  void initSharedVBOs();

  int m_lastID;
  int m_current_row;
  int m_current_col;

};

