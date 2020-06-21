#include "MainWidget.h"

namespace fs = std::filesystem;

MainWidget::MainWidget(DataContainer* datacontainer, InputForm* input_form, QWidget* parent)
  : QOpenGLWidget(parent)
{
  m_datacontainer = datacontainer;
  m_input_form = input_form;
  setFocusPolicy(Qt::StrongFocus);
}

void MainWidget::on_synapse_distance_slider_changed(int value)
{
  double distance = ((double)value / 100.0) * 6;

  for (auto const& [id, widget] : m_widgets) 
  {
    if (id != 0) 
    {
      widget->update_synapse_distance_threshold(distance);
    }
  }
}

bool MainWidget::addGLWidget(int ID, bool isOverviewWidget)
{
  QString name;
  if (isOverviewWidget)
  {
    name = "Overview";
  }
  else
  {
    name = m_datacontainer->getObjectsMapPtr()->at(ID)->getName().c_str();
  }
  QGroupBox* groupBox = new QGroupBox(name, this);
  QVBoxLayout* vbox = new QVBoxLayout;

  GLWidget* widget = new GLWidget(ID, m_shared_resources, isOverviewWidget, this);
  widget->init(m_input_form); // todo delete dependecy of input form later
  vbox->addWidget(widget);
  
  groupBox->setLayout(vbox);
  m_layout->addWidget(groupBox, m_current_row, m_current_col);

  m_widgets[ID] = widget;  

  if (m_current_col < m_max_cols - 1) 
  {
    m_current_col++;
  }
  else {
    m_current_row++;
    m_current_col = 0;
  }

  return true;
}

bool MainWidget::deleteWidget(int ID)
{
  return false;
}

bool MainWidget::addInfoVisWidget(int ID)
{
  fs::path current_path = fs::current_path();
  qDebug() << current_path.parent_path().c_str();

  // process html path 
  QString base_path = QString(current_path.parent_path().string().c_str());
  base_path.replace("\\", "/");
  QString url = base_path + QString("/src/index.html");

  InfoVisWidget* widget = new InfoVisWidget(QUrl::fromLocalFile(url));

  QGroupBox* groupBox = new QGroupBox("InfoVisView", this);
  QVBoxLayout* vbox = new QVBoxLayout;

  vbox->addWidget(widget);

  groupBox->setLayout(vbox);
  m_layout->addWidget(groupBox, m_current_row, m_current_col);

  if (m_current_col < m_max_cols - 1)
  {
    m_current_col++;
  }
  else {
    m_current_row++;
    m_current_col = 0;
  }

  return true;
}

void MainWidget::keyPressEvent(QKeyEvent* event)
{
  qDebug() << "Key pressed";
  switch (event->key())
  {
  case(Qt::Key_A):
    addInfoVisWidget(m_lastID);
    break;
  }
}

void MainWidget::initializeGL()
{
  initializeOpenGLFunctions();

  m_layout = new QGridLayout(this);

  // setup shared resources
  initSharedVBOs();
 
  // add first widget
  addGLWidget(0, true);


}

void MainWidget::paintGL()
{
  glClearColor(1.0, 1.0, 1.0, 1.0);
}

void MainWidget::resizeGL(int width, int height)
{
  // do nothing
}

void MainWidget::initSharedVBOs()
{
  initSharedMeshVBOs();
  //initSharedSliceVBOs();

}

void MainWidget::load3DTexturesFromRaw(QString path, GLuint& texture, GLenum texture_unit, int sizeX, int sizeY, int sizeZ)
{
  unsigned int size = sizeX * sizeY * sizeZ;
  unsigned char* rawData = (unsigned char*)m_datacontainer->loadRawFile(path, size);

  //load data into a 3D texture
  glGenTextures(1, &texture);
  glActiveTexture(texture_unit);
  glBindTexture(GL_TEXTURE_3D, texture);

  // set the texture parameters
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  //glTexImage3D(GL_TEXTURE_3D, 0, GL_R8, sizeX, sizeY, sizeZ, 0, GL_RED, GL_UNSIGNED_BYTE, rawData);

  delete[] rawData;
}

void MainWidget::initSharedSliceVBOs()
{
  QString image_volume_path("C:/Users/jtroidl/Desktop/6mice_sp_bo/m3/m3_stack.raw");

  float sliceVertices[] =
  {
    // vertices                  // uv - coords
    0.0,          0.0,          0.0,              0.0, 0.0,
    0.0,          MESH_MAX_Y,   0.0,              0.0, 1.0,
    0.0,          0.0,          MESH_MAX_Z,       1.0, 0.0,

    0.0,          MESH_MAX_Y,   MESH_MAX_Z,       1.0, 1.0,
    0.0,          MESH_MAX_Y,   0.0,              0.0, 1.0,
    0.0,          0.0,          MESH_MAX_Z,       1.0, 0.0
  };

  m_slice_vertex_vbo = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
  m_slice_vertex_vbo.create();
  m_slice_vertex_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
  m_slice_vertex_vbo.bind();

  m_slice_vertex_vbo.allocate(sliceVertices, sizeof(sliceVertices));

  m_slice_vertex_vbo.release();

  m_shared_resources.slice_vertex_vbo = &m_slice_vertex_vbo;
}

void MainWidget::initSharedMeshVBOs()
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
  m_shared_resources.mesh_normal_vbo = &m_mesh_normal_vbo;
  m_shared_resources.index_count = neurites_index_count;
}