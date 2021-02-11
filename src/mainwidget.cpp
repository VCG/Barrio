#include "MainWidget.h"

#define STB_IMAGE_IMPLEMENTATION

MainWidget::MainWidget(DataContainer* datacontainer, InputForm* input_form, QWidget* parent)
  : QOpenGLWidget(parent)
{
  m_datacontainer = datacontainer;
  m_input_form = input_form;
  m_number_of_entities = NumberOfEntities::LOW;
  setFocusPolicy(Qt::StrongFocus);

  m_main_layout = new QGridLayout(this);

  m_abstraction_space = new AbstractionSpace(datacontainer);
}

double MainWidget::on_synapse_distance_slider_changed(int value)
{
  double distance = ((double)value / 100.0) * sqrt(3) * MESH_MAX_X;
  
  m_abstraction_space->setThresholdDistance(distance);

  for (auto const& [id, widget] : m_opengl_views) 
  {
    if (id != 0) 
    {
      widget->update_synapse_distance_threshold(distance);
    }
  }

  updateInfoVisViews();

  return distance;
}

void MainWidget::on_opacity_slider_changed(int value)
{
  m_shared_resources.cell_opacity = (float)value / 100.0;

  for (auto const& [id, widget] : m_opengl_views)
  {
    widget->updateVisParameters();
  }
}

void MainWidget::on_slice_position_slider_changed(int value)
{
  set_slice_position(value);
}

void MainWidget::set_slice_position(int value)
{
  m_shared_resources.slice_depth = (float)value / (100.0 / MESH_MAX_Z);
}

void MainWidget::on_widget_close_button_clicked()
{
  qDebug() << "Close button pressed";

  QPushButton* button = qobject_cast<QPushButton*>(sender());
  QWidget* widget_to_delete = button->parentWidget()->parentWidget();
  
  int id_to_delete = 0;
  for (auto& i : m_groupboxes) {
    if (i.second == widget_to_delete) {
      id_to_delete = i.first;
      break; // to stop searching
    }
  }

  QList<int> currentlySelectedIDs = getSelectedIDs();

  deleteAllWidgets(false);

  for each (int ID in currentlySelectedIDs)
  {
    if (ID != id_to_delete)
    {
      addWidgetGroup(ID, false);
    }
  }

  updateInfoVisViews();
}

void MainWidget::keyPressEvent(QKeyEvent* event)
{
  //switch (event->key()) {
  //  case(Qt::Key_S): // reset
  //    for (auto const& [id, widget] : m_opengl_views)
  //    {
  //      widget->resetCamera();
  //    }
  //    break;
  //}
}

void MainWidget::addCloseButtonToWidget(QGroupBox* groupBox)
{
  QFrame* frame = new QFrame;
  frame->setMaximumHeight(40);
  QHBoxLayout* horizontal_layout = new QHBoxLayout();

  QPushButton* closeButton = new QPushButton("X", groupBox);
  closeButton->setMaximumSize(QSize(20, 20));

  horizontal_layout->addStretch();
  horizontal_layout->addWidget(closeButton);

  frame->setLayout(horizontal_layout);

  groupBox->layout()->addWidget(frame);
  connect(closeButton, SIGNAL(released()), this, SLOT(on_widget_close_button_clicked()));

}

bool MainWidget::addWidgetGroup(int ID, bool isOverviewWidget)
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

  
  m_abstraction_space->addToSelectedIndices(ID);
  m_lastID = ID;

  // low configuration
  if (m_number_of_entities == NumberOfEntities::LOW) 
  {
    QGroupBox* groupBox = new QGroupBox(name, this);
    QVBoxLayout* vbox = new QVBoxLayout;
    groupBox->setLayout(vbox);
    m_groupboxes[ID] = groupBox;

    addCloseButtonToWidget(groupBox);

    addInfoVisWidget(ID, groupBox, m_vis_methods.method->clone());

    QFrame* line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    groupBox->layout()->addWidget(line);

    m_seperation_elements[ID] = line;
    addGLWidget(ID, groupBox, isOverviewWidget);

    QSize size = m_main_layout->totalSizeHint();
    int number_of_columns = m_main_layout->columnCount();
    for (auto const& [id, box] : m_groupboxes)
    {
      box->setMinimumWidth((size.width() - 25) / (number_of_columns + 1));
    }
    
    m_main_layout->addWidget(groupBox, m_current_row, m_current_col);
  }
  // medium configuration
  else if(m_number_of_entities == NumberOfEntities::MEDIUM)
  {
    updateInfoVisViews();

    QGroupBox* groupBox = new QGroupBox(name, this);
    QVBoxLayout* vbox = new QVBoxLayout;
    groupBox->setLayout(vbox);
    m_groupboxes[ID] = groupBox;

    addCloseButtonToWidget(groupBox);

    addGLWidget(ID, groupBox, isOverviewWidget);
  }
  // high configuration
  else 
  {
    updateInfoVisViews();

    QGroupBox* groupBox = new QGroupBox(name, this);
    QVBoxLayout* vbox = new QVBoxLayout;
    groupBox->setLayout(vbox);
    m_groupboxes[ID] = groupBox;

    addCloseButtonToWidget(groupBox);

    addGLWidget(ID, groupBox, isOverviewWidget);
  }

  if (m_current_col < m_max_cols - 1) 
  {
    m_current_col++;
  }
  else {
    m_current_row = m_current_row + 1;
    m_current_col = 0;
  }

  m_number_of_selected_structures++;
  
  return true;
}

//bool MainWidget::deleteInfoVisWidget(int ID)
//{
//  QGroupBox* widget = m_info_vis_boxes[ID];
//  widget->hide();
//  m_gl_layout->removeWidget(widget);
//  delete widget;
//  
//  return true;
//}

Vis MainWidget::getVisInfo(int id)
{
  return m_abstraction_space->m_vis_space.at(id);
}

bool MainWidget::deleteAllInfoVisWidgets()
{
  // clear view map
  for (auto it = m_infovis_views.cbegin(); it != m_infovis_views.cend();)
  {
    QWebEngineView* webengine = (*it).second->getWebEngineView();

    webengine->hide();
    webengine->layout()->removeWidget(webengine);
    delete webengine;

    it = m_infovis_views.erase(it);
  }

  //for (auto it = m_seperation_elements.cbegin(); it != m_seperation_elements.cend(); it++)
  //{
  //  QFrame* line = (*it).second;

  //  //line->hide();
  //  //line->layout()->removeWidget(line);
  //  //delete line;
  //}

  m_seperation_elements.clear();

  // medium detail info vis view, if exists delete
  if (m_groupboxes.find(-1) != m_groupboxes.end())
  {
    auto it = m_groupboxes.find(-1);

    QGroupBox* groupBox = (*it).second;
    groupBox->hide();
    m_main_layout->removeWidget(groupBox);
    delete groupBox;

    m_groupboxes.erase(it);
  }

  // low detail info vis view, if exists delete
  if (m_groupboxes.find(-2) != m_groupboxes.end())
  {
    auto it = m_groupboxes.find(-2);

    QGroupBox* groupBox = (*it).second;
    groupBox->hide();
    m_main_layout->removeWidget(groupBox);
    delete groupBox;

    m_groupboxes.erase(it);
  }


  return true;
}

bool MainWidget::deleteAllGLWidgets()
{
  // clear group box map
  //for (auto it = m_gl_boxes.cbegin(); it != m_gl_boxes.cend();)
  //{
  //  int ID = (*it).first;
  //  QGroupBox* widget = (*it).second;
  //  widget->hide();
  //  m_gl_layout->removeWidget(widget);
  //  delete widget;
  //  it = m_gl_boxes.erase(it);
  //}

  //// clear group box map
  //for (auto it = m_opengl_views.cbegin(); it != m_opengl_views.cend();)
  //{
  //  it = m_opengl_views.erase(it);
  //}

  //m_current_col = 0;
  //m_current_row = 0;

  return false;
}

bool MainWidget::deleteAllWidgets(bool deleteGeneralInfoVisWidgets)
{
  for (auto it = m_groupboxes.cbegin(); it != m_groupboxes.cend();)
  {
    int ID = (*it).first;
    if (ID > 0 || deleteGeneralInfoVisWidgets)
    {
      m_abstraction_space->removeFromSelectedindices(ID);

      QGroupBox* groupBox = (*it).second;

      groupBox->hide();
      m_main_layout->removeWidget(groupBox);
      delete groupBox;

      // delete respective element in gl map
      if (m_opengl_views.find(ID) != m_opengl_views.end())
      {
        auto gl_it = m_opengl_views.find(ID);
        m_opengl_views.erase(gl_it);
      }

      // delete respective elements in the infovis map
      if (m_infovis_views.find(ID) != m_infovis_views.end())
      {
        auto infovis_it = m_infovis_views.find(ID);
        m_infovis_views.erase(infovis_it);
      }

      it = m_groupboxes.erase(it);
    }
    else 
    {
      ++it;
    }
  }

  m_current_col = 0;
  m_current_row = 0;

  return true;
}

bool MainWidget::addInfoVisWidget(int ID, QGroupBox* groupBox, IVisMethod* visMethod)
{
  QWebEngineView* widget = visMethod->initVisWidget(ID, m_specific_vis_parameters);
  groupBox->layout()->addWidget(widget);

  else if(m_number_of_entities == NumberOfEntities::MEDIUM)
  {
    m_main_layout->addWidget(groupBox, 0, 0, 1, -1);
  }
  else if(m_number_of_entities == NumberOfEntities::HIGH)
  {
    m_main_layout->addWidget(groupBox, 0, 0, 1, -1);
  }

  m_infovis_views[ID] = visMethod;
  
  return true;
}

bool MainWidget::addGLWidget(int ID, QGroupBox* groupBox, bool isOverviewWidget)
{
  GLWidget* widget = new GLWidget(ID, &m_shared_resources, isOverviewWidget, this);
  widget->init(m_datacontainer);
  groupBox->layout()->addWidget(widget);

  m_opengl_views[ID] = widget;

  return true;
}

//void MainWidget::setupMainWidget(VisConfiguration vis_config)
//{
//  m_vis_methods = m_abstraction_space->configureVisMethods(vis_config);
//  qDebug() << "Decided on Vis methods";
//}

//SelectedVisMethods MainWidget::setThumbnailIcons(VisConfiguration vis_config)
//{
//  //m_vis_methods = m_abstraction_space->configureVisMethods(vis_config);
//
//  //// reload all widgets
//  //QList<int> currentlySelectedIDs = getSelectedIDs();
//  //deleteAllWidgets(false);
//  //for each (int ID in currentlySelectedIDs)
//  //{
//  //   addWidgetGroup(ID, false);
//  //}
//  //
//  //return m_vis_methods;
//}

void MainWidget::showSlice(bool showSlice)
{
  m_shared_resources.show_slice = showSlice;
}

void MainWidget::updateInfoVisViews()
{
  for (auto const& [id, view] : m_infovis_views)
  {
    view->update();
    view->getWebEngineView()->reload();
  }
}

void MainWidget::updateGroupBoxStyle()
{
  QVector<int>* groupBoxes = m_shared_resources.highlighted_group_boxes;
  for (auto const& [hvgx, box] : m_groupboxes)
  {
    box->setObjectName(QString::number(hvgx));
    if (groupBoxes->contains(hvgx))
    {
      box->setStyleSheet("QGroupBox#" + QString::number(hvgx) + "{ border: 2px solid orange; }");
    }
    else
    {
      box->setStyleSheet("QGroupBox#" + QString::number(hvgx) + "{ border: 1px solid lightgray; }");
    }
  }
}

void MainWidget::setNumberOfBinsForHistogram(int bins)
{
  m_specific_vis_parameters.number_of_bins = bins;
  for (auto const& [id, view] : m_infovis_views)
  {
    view->setSpecificVisParameters(m_specific_vis_parameters);
    view->getWebEngineView()->reload();
  }
}

void MainWidget::setColormap(QString name)
{
  m_selected_colormap = name;

  QImage* colormap = getColorMap(name);
  m_specific_vis_parameters.colors = getColorValues(colormap);

  for (auto const& [id, view] : m_infovis_views)
  {
    view->setSpecificVisParameters(m_specific_vis_parameters);
    view->getWebEngineView()->reload();
  }
  
  initColormaps();
}

void MainWidget::setVisMethod(Vis vis)
{
  m_vis_methods.method = m_abstraction_space->decideOnVisMethod(vis);
  if (vis.scale == NumberOfEntities::MEDIUM)
  {
    m_number_of_entities = NumberOfEntities::MEDIUM;
    int medium_entities_id = -1;
    
    deleteAllInfoVisWidgets();

    QGroupBox* groupBox = new QGroupBox(m_medium_detail_name, this);
    QVBoxLayout* vbox = new QVBoxLayout;
    groupBox->setLayout(vbox);
    m_groupboxes[medium_entities_id] = groupBox;

    
    addInfoVisWidget(medium_entities_id, groupBox, m_vis_methods.method->clone());
  }
  else if(vis.scale == NumberOfEntities::LOW)
  {
     m_number_of_entities = NumberOfEntities::LOW;
     QList<int> currentlySelectedIDs = getSelectedIDs();

     deleteAllWidgets(true);

     for each (int ID in currentlySelectedIDs)
     {
       addWidgetGroup(ID, false);
     }
  }
 
  else if (vis.scale == NumberOfEntities::HIGH) 
  {
    int high_entities_id = -2;
    m_number_of_entities = NumberOfEntities::HIGH;

    deleteAllInfoVisWidgets();

    QGroupBox* groupBox = new QGroupBox(m_low_detail_name, this);
    QVBoxLayout* vbox = new QVBoxLayout;
    groupBox->setLayout(vbox);
    m_groupboxes[high_entities_id] = groupBox;
    
    addInfoVisWidget(high_entities_id, groupBox, m_vis_methods.method->clone());
  }
}

//void MainWidget::keyPressEvent(QKeyEvent* event)
//{
// /* qDebug() << "Key pressed";
//  switch (event->key())
//  {
//  case(Qt::Key_A):
//    addInfoVisWidget(m_lastID);
//    break;
//  }*/
//}

void MainWidget::initializeGL()
{
  initializeOpenGLFunctions();

  // setup shared resources
  initSharedVBOs();
  initColormaps();
  init3DVolumeTexture();

  m_shared_resources.highlighted_objects = &m_abstraction_space->m_global_vis_parameters.highlighted_objects;
  m_shared_resources.highlighted_group_boxes = &m_abstraction_space->m_global_vis_parameters.highlighted_group_boxes;


  // add first widget
  //addGLWidget(0, true);

}



void MainWidget::resizeGL(int width, int height)
{
}

//bool MainWidget::initOpenGLFunctions()
//{
//  return false;
//}

void MainWidget::paintGL()
{
  glClearColor(1.0, 1.0, 1.0, 1.0);
  updateGroupBoxStyle();
  update();
}

//void MainWidget::resizeGL(int width, int height)
//{
//  // do nothing
//}

QList<int> MainWidget::getSelectedIDs()
{
  QList<int> selectedIDs;
  for (const auto [ID, widget] : m_opengl_views)
  {
    selectedIDs.push_back(ID);
  }
  return selectedIDs;
}

SelectedVisMethods MainWidget::getSelectedVisMethods()
{
  return m_vis_methods;
}

void MainWidget::initSharedVBOs()
{
  initSharedMeshVBOs();
}

void MainWidget::initColormaps()
{
  makeCurrent();

  QImage* colormap = getColorMap(m_selected_colormap);
  m_specific_vis_parameters.colors = getColorValues(colormap);

  init_1D_texture(m_mito_cell_distance_colormap, GL_TEXTURE1, colormap->bits(), colormap->width());
  m_shared_resources.mito_cell_distance_colormap = &m_mito_cell_distance_colormap;
}

void MainWidget::init3DVolumeTexture()
{
  QString image_volume_path(PATH + "data/m3_data/m3_stack.raw");

  load3DTexturesFromRaw(image_volume_path, m_image_stack_texture, GL_TEXTURE0, DIM_X, DIM_Y, DIM_Z);
  m_shared_resources.image_stack_volume = &m_image_stack_texture;
}

void MainWidget::init_1D_texture(GLuint& texture, GLenum texture_unit, GLvoid* data, int size)
{
  glGenTextures(1, &texture);

  glActiveTexture(texture_unit);
  glBindTexture(GL_TEXTURE_1D, texture);

  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

  glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, size, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
}

void MainWidget::load3DTexturesFromRaw(QString path, GLuint& texture, GLenum texture_unit, int sizeX, int sizeY, int sizeZ)
{
  unsigned int size = sizeX * sizeY * sizeZ;
  unsigned char* rawData = (unsigned char*)m_datacontainer->loadRawFile(path, size);

  FILE* pFile = fopen(path.toStdString().c_str(), "rb");

  if (rawData) 
  {
    unsigned char* pVolume = new unsigned char[size];
    
    fclose(pFile);

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

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage3D(GL_TEXTURE_3D, 0, GL_R8, sizeX, sizeY, sizeZ, 0, GL_RED, GL_UNSIGNED_BYTE, rawData);

    delete[] rawData;
  }
}

QImage* MainWidget::getColorMap(QString name)
{
  // get path
  QDir dir = QDir::currentPath();
  dir.cdUp();
  dir.cd("src");
  dir.cd("colormaps");
  return new QImage(dir.path() + "/" + name);
}

QString MainWidget::getColorValues(QImage* image)
{
  QJsonArray colors;
  for (int i = 0; i < image->width(); i++)
  {
    QColor color(image->pixel(i, 0));
    colors.push_back(color.name());
  }

  QJsonDocument doc(colors);
  return doc.toJson(QJsonDocument::Compact);
}

void MainWidget::initSharedSlice()
{
  float sliceVertices[] =
  {
    // vertices                                   // uv - coords
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

  int skeleton_indices_size = m_datacontainer->getSkeletonIndices()->size();

  m_skeleton_index_vbo = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
  m_skeleton_index_vbo.create();
  m_skeleton_index_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
  m_skeleton_index_vbo.bind();
  m_skeleton_index_vbo.allocate(m_datacontainer->getSkeletonIndices()->data(), skeleton_indices_size * sizeof(GLuint));
  m_skeleton_index_vbo.release();

  // setup shared resource container
  m_shared_resources.vertex_vbo = &m_mesh_vertex_vbo;
  m_shared_resources.mesh_index_vbo = &m_mesh_index_vbo;
  m_shared_resources.skeleton_index_vbo = &m_skeleton_index_vbo;
  m_shared_resources.mesh_index_count = neurites_index_count;
  m_shared_resources.skeleton_index_count = skeleton_indices_size;
}


