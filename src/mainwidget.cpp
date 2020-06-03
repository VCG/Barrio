#include "MainWidget.h"

MainWidget::MainWidget(InputForm* input_form, QWidget* parent)
  : QOpenGLWidget(parent)
{
  m_datacontainer = new DataContainer(input_form);
  m_input_form = input_form;
  setFocusPolicy(Qt::StrongFocus);
}

bool MainWidget::addGLWidget(int ID, int row, int col)
{

  GLWidget* widget = new GLWidget(ID, m_shared_resources, this);
  
  widget->init(m_input_form); // todo delete dependecy of input form later

  m_widgets[ID] = widget;
  m_layout->addWidget(widget, row, col);

  return true;
}

bool MainWidget::deleteWidget(int ID)
{
  return false;
}

void MainWidget::keyPressEvent(QKeyEvent* event)
{
  qDebug() << "Key pressed";
  switch (event->key())
  {
  case(Qt::Key_A):
    m_lastID++;
    m_current_row++;
    addGLWidget(m_lastID, m_current_row, m_current_col);
    break;
  }
}

void MainWidget::initializeGL()
{
  initializeOpenGLFunctions();
  
  qDebug() << "init mainwidget";

  m_layout = new QGridLayout(this);

  // setup shared resources
  initSharedVBOs();

  m_current_row = 0;
  m_current_col = 0;
  m_lastID = 10;



  // add first widget
  addGLWidget(m_lastID, m_current_row, m_current_col);

 /* addGLWidget(1, 0, 1);

  addGLWidget(2, 1, 0);

  addGLWidget(3, 1, 1);*/

  //addGLWidget(4, 2, 0);

  //addGLWidget(5, 2, 1);

}

void MainWidget::paintGL()
{
  qDebug() << "Trying to paint mainwidget";
  glClearColor(1.0, 0.0, 0.0, 1.0);
  // do nothing
}

void MainWidget::resizeGL(int width, int height)
{
  qDebug() << "Trying to resize mainwidget";
  // do nothing
}

void MainWidget::initSharedVBOs()
{
  m_mesh_index_vbo = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
  m_mesh_index_vbo.create();
  m_mesh_index_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
  m_mesh_index_vbo.bind();

  int astro_index_count = m_datacontainer->getIndicesSizeByObjectType(Object_t::ASTROCYTE);
  int neurites_index_count = m_datacontainer->getMeshIndicesSize() - astro_index_count;

  m_mesh_index_vbo.allocate(neurites_index_count * sizeof(GLuint));

  // initialize index buffers
  int offset = 0;
  std::map<int, Object*>* objects_map = m_datacontainer->getObjectsMapPtr();

  //iterate over all objects and add indices to the VBO
  for (auto iter = objects_map->rbegin(); iter != objects_map->rend(); iter++)
  {
    Object* object_p = (*iter).second;

    int obj_index_count = object_p->get_indices_size() * sizeof(GLuint);
    m_mesh_index_vbo.write(offset, object_p->getIndexData(), obj_index_count);
    offset += obj_index_count;
  }
  m_mesh_index_vbo.release();

  Mesh* mesh = m_datacontainer->getMesh();

  // create and allocate vertex vbo
  m_mesh_vertex_vbo = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
  m_mesh_vertex_vbo.create();
  m_mesh_vertex_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
  m_mesh_vertex_vbo.bind();
  m_mesh_vertex_vbo.allocate(mesh->getVerticesList()->data(), mesh->getVerticesList()->size() * sizeof(VertexData));
  m_mesh_vertex_vbo.release();

  // create and allocate index vbo
  m_mesh_normal_vbo = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
  m_mesh_normal_vbo.create();
  m_mesh_normal_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
  m_mesh_normal_vbo.bind();
  m_mesh_normal_vbo.allocate(mesh->getNormalsList()->data(), mesh->getNormalsList()->size() * sizeof(QVector4D));
  m_mesh_normal_vbo.release();

  // setup shared resource container
  m_shared_resources.mesh_index_vbo = &m_mesh_index_vbo;
  m_shared_resources.mesh_vertex_vbo = &m_mesh_vertex_vbo;
  m_shared_resources.mesh_normal_vbo = &m_mesh_index_vbo;
  m_shared_resources.index_count = neurites_index_count;

}
