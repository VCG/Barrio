/*
 * FinalMatrix = Projection * View * Model
 * Model = RotationAroundOrigin * TranslationFromOrigin * RotationAroundObjectCenter
 */
#include <QResource>
#include <QTimer>

#include "glwidget.h"
#include "colors.h"

GLWidget::GLWidget(int hvgx_id, SharedGLResources resources, bool isOverviewWidget, QWidget* parent)
  : QOpenGLWidget(parent),
  m_yaxis(0),
  m_xaxis(0),
  m_FDL_running(false),
  m_2D(false),
  m_hover(false)
{
  m_distance = 1.0;
  m_rotation = QQuaternion();
  //reset rotation
  m_rotation.setScalar(1.0f);
  m_rotation.setX(0.0f);
  m_rotation.setY(0.0f);
  m_rotation.setZ(0.0f);
  //reset translation
  m_translation = QVector3D(0.0, 0.0, 0.0);

  m_lockRotation2D_timer = new QTimer(this);
  connect(m_lockRotation2D_timer, SIGNAL(timeout()), this, SLOT(update()));
  m_lockRotation2D_timer->start(0);

  m_lockRotation2D_timer = new QTimer(this);
  connect(m_lockRotation2D_timer, SIGNAL(timeout()), this, SLOT(lockRotation2D()));

  m_auto_rotation_timer = new QTimer(this);
  connect(m_auto_rotation_timer, SIGNAL(timeout()), this, SLOT(startRotation()));

  connect(this, SIGNAL(aboutToResize()), this, SLOT(prepareResize()));

  m_active_graph_tab = 0;
  //setFocusPolicy(Qt::StrongFocus);
  m_hide_toggle = false;

  m_auto_rotate = false;
  m_rot_ydiff = 1;

  m_xy_slice_z = 0.5f * MESH_MAX_Z;

  m_shared_resources = resources;
  m_selected_hvgx_id = hvgx_id;


  headPtrClearBuf = new std::vector<GLuint>();
  m_init = false;

  m_height = 0;
  m_width = 0;

  m_is_overview_widget = isOverviewWidget;
}

GLWidget::~GLWidget()
{
  qDebug() << "~GLWidget()";

  makeCurrent();
  delete m_2dspace;
  delete m_graphManager;
  delete m_data_containter;
  delete m_glycogenAnalysisManager;
  doneCurrent();
}

void GLWidget::init(InputForm* input_form)
{
  // 2D abstraction space, with intervals properties intializaiton and geometry
  m_2dspace = new AbstractionSpace(100, 100);

  m_data_containter = new DataContainer(input_form);

  // objects manager with all objects data
  m_opengl_mngr = new OpenGLManager(m_data_containter, m_2dspace);

  // graph manager with 4 graphs and 2D space layouted data
  m_graphManager = new GraphManager(m_data_containter, m_opengl_mngr);

}



void GLWidget::updateMVPAttrib(QOpenGLShaderProgram* program)
{
  // calculate model view transformation
  // world/model matrix: determines the position and orientation of an object in 3D space
  m_mMatrix.setToIdentity();

  // Center Zoom
  m_mMatrix.translate(m_center);
  m_mMatrix.scale(m_distance);
  m_mMatrix.translate(-1.0 * m_center);

  //// Translation
  m_mMatrix.translate(m_translation);

  // Model Matrix without rotation
  m_model_noRotation.setToIdentity();
  m_model_noRotation = m_mMatrix;

  // Rotation
  m_rotationMatrix.setToIdentity();
  m_rotationMatrix.translate(m_center);
  m_rotationMatrix.rotate(m_rotation);
  m_rotationMatrix.translate(-1.0 * m_center);
  m_mMatrix *= m_rotationMatrix;

  int rMatrix = program->uniformLocation("rMatrix");
  if (rMatrix >= 0) program->setUniformValue(rMatrix, m_rotationMatrix);

  int mMatrix = program->uniformLocation("mMatrix");
  if (mMatrix >= 0) program->setUniformValue(mMatrix, m_mMatrix);
  GL_Error();

  int vMatrix = program->uniformLocation("vMatrix");
  if (vMatrix >= 0) program->setUniformValue(vMatrix, m_vMatrix);
  GL_Error();

  int pMatrix = program->uniformLocation("pMatrix");
  if (pMatrix >= 0) program->setUniformValue(pMatrix, m_projection);
  GL_Error();

  int mNodes = program->uniformLocation("maxNodes");
  if (mNodes >= 0) program->setUniformValue(mNodes, m_maxNodes);
  GL_Error();

  //const qreal retinaScale = devicePixelRatio();
  //QVector4D viewport = QVector4D(0, 0, width() * retinaScale, height() * retinaScale);

  //int max_astro_coverage = m_data_containter->getMaxAstroCoverage();
  //int max_volume = m_data_containter->getMaxVolume();

  //// graph model matrix without rotation, apply rotation to nodes directly
  //m_uniforms = { m_yaxis, m_xaxis, m_mMatrix.data(), m_vMatrix.data(), m_projection.data(), m_model_noRotation.data(),
  //  m_rotationMatrix, viewport, max_volume, max_astro_coverage, 0.0001, m_xy_slice_z};
  //m_opengl_mngr->updateUniformsData(m_uniforms);
}

void GLWidget::initializeGL()
{
  qDebug() << "initializeGL";
  m_performaceMeasure.startTimer();

  initializeOpenGLFunctions();
  
  QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();
  
  f->glClearColor(1.0, 1.0, 1.0, 1.0);

  m_mesh_program = new QOpenGLShaderProgram;
  m_mesh_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/mesh_vert_simplified.glsl");
  m_mesh_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/mesh_frag_simplified.glsl");
  m_mesh_program->link();

  //f->glEnable(GL_CULL_FACE);
  //f->glCullFace(GL_BACK);
  f->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  int success = m_mesh_program->bind();
  
  m_mesh_vao.create();
 
  m_mesh_vao.bind();


  // bind vbos to vao
  m_shared_resources.mesh_vertex_vbo->bind();

  // setting up vertex attributes
  GLsizei stride = sizeof(VertexData);

  // mesh_vtx
  int offset = 0;
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, stride, 0);

  // distance to cell
  offset += sizeof(QVector4D);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);

  // structure type
  offset += sizeof(GL_FLOAT);
  glEnableVertexAttribArray(2);
  glVertexAttribIPointer(2, 1, GL_INT, stride, (GLvoid*)offset);

  // hvgx ID
  offset += sizeof(GL_INT);
  glEnableVertexAttribArray(3);
  glVertexAttribIPointer(3, 1, GL_INT, stride, (GLvoid*)offset);

  m_shared_resources.mesh_normal_vbo->bind();
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, 0);

  m_mesh_vao.release();


  mesh_shader_pass_idx_1 = glGetSubroutineIndex(m_mesh_program->programId(), GL_FRAGMENT_SHADER, "pass1");
  mesh_shader_pass_idx_2 = glGetSubroutineIndex(m_mesh_program->programId(), GL_FRAGMENT_SHADER, "pass2");
  GL_Error();

  // Set up a  VAO for the full-screen quad
  GLfloat verts[] = { -1.0f, -1.0f, 0.0f, 1.0,
                       1.0f, -1.0f, 0.0f, 1.0,
                       1.0f,  1.0f, 0.0f, 1.0,
                      -1.0f,  1.0f, 0.0f, 1.0 };

  GLuint bufHandle;
  glGenBuffers(1, &bufHandle);
  glBindBuffer(GL_ARRAY_BUFFER, bufHandle);
  glBufferData(GL_ARRAY_BUFFER, 4 * 4 * sizeof(GLfloat), verts, GL_STATIC_DRAW);

  // Set up the vertex array object
  m_fsQuad_vao.create();
  m_fsQuad_vao.bind();

  glBindBuffer(GL_ARRAY_BUFFER, bufHandle);
  glEnableVertexAttribArray(0);  // Vertex position
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
  

  m_fsQuad_vao.release();
  GL_Error();

  initVisibilitySSBO();
  
  m_mesh_program->release();

  if (m_FDL_running) {
    stopForecDirectedLayout();
  }

  m_lockRotation2D_timer->start(500);
}


void GLWidget::paintGL()
{
  startRotation();
    
  bool success = m_mesh_program->bind();
  updateMVPAttrib(m_mesh_program);

  clearBuffers();
  pass1();
  glFlush();
  pass2();
  
  GL_Error();

  m_mesh_program->release();
}

void GLWidget::resizeGL(int w, int h)
{
  const qreal retinaScale = devicePixelRatio();
  h = (h == 0) ? 1 : h;

  m_width = w * retinaScale;
  m_height = h * retinaScale;
  

  qDebug() << "Resize Widget: " << m_width << ", " << m_height;

  // Calculate aspect ratio
  m_uniforms.viewport = QVector4D(0, 0, m_width, m_height);

  qreal aspect = retinaScale * qreal(w) / qreal(h ? h : 1);
  m_projection.setToIdentity();
  m_projection.perspective(45.0, aspect, 1.0, 10 * MESH_MAX_Z);

  // set up view
  // view matrix: transform a model's vertices from world space to view space, represents the camera
  if (m_is_overview_widget) 
  {
    m_center = QVector3D(MESH_MAX_X / 2.0, MESH_MAX_Y / 2.0, MESH_MAX_Z / 2.0);
  }
  else
  {
    Object* selectedObject = m_data_containter->getObjectsMapPtr()->at(m_selected_hvgx_id);
    m_center = selectedObject->getCenter().toVector3D();
  }
 
  m_cameraUpDirection = QVector3D(0.0, 1.0, 0.0);
  m_eye = QVector3D(MESH_MAX_X / 2.0, MESH_MAX_Y / 2.0, 2.0 * MESH_MAX_Z);
  
  m_vMatrix.setToIdentity();
  m_vMatrix.lookAt(m_eye, m_center, m_cameraUpDirection);


  initMeshShaderStorage(m_width, m_height);

  update();
}

int GLWidget::pickObject(QMouseEvent* event)
{
  const qreal retinaScale = devicePixelRatio();
  GLint viewport[4]; //  return of glGetIntegerv() -> x, y, width, height of viewport
  glGetIntegerv(GL_VIEWPORT, viewport);

  int x = event->x() * retinaScale;
  int y = viewport[3] - event->y() * retinaScale;
  int hvgxID = m_opengl_mngr->processSelection(x, y);
  if (hvgxID == 16777215)
  {
    hvgxID = 0;
  }
  setHoveredID(hvgxID);
  std::string name = m_data_containter->getObjectName(hvgxID);
  QString oname = QString::fromUtf8(name.c_str());
  setHoveredName(oname);
  return hvgxID;
}

void GLWidget::mousePressEvent(QMouseEvent* event)
{
  m_lastPos = event->pos();
  setFocus();

  makeCurrent();

  /*m_opengl_mngr->renderSelection(&m_uniforms);

  int hvgxID = pickObject(event);

  if (hvgxID == 0)    goto quit;
  m_opengl_mngr->highlightObject(hvgxID);

  if (event->modifiers() == Qt::ControlModifier) {

    if (m_selectedObjects.size() == 0) {
      clearRowsTable();
    }

    if (m_selectedObjects.find(hvgxID) == m_selectedObjects.end()) {
      m_selectedObjects.insert(hvgxID);
      insertInTable(hvgxID);
    }
  }*/

quit:
  doneCurrent();

  event->accept();
}

void GLWidget::insertInTable(int hvgxID)
{

  std::string name = m_data_containter->getObjectName(hvgxID);
  if (name == "Unknown")
    return;

  QString oname = QString::fromUtf8(name.c_str());

  QList<QStandardItem*> items;
  items.append(new QStandardItem(QString::number(hvgxID)));
  items.append(new QStandardItem(oname));
  object_clicked(items);

}

void GLWidget::lockRotation2D()
{
  if (m_FDL_running == true)
    return;

  m_lockRotation2D_timer->stop();
  m_FDL_running = true;   // run force layout


  // start a timer, and reset it whenever we are here
  // once we exceed threshold start force layouted
  // if we are below x < 50 and y < 50
  //updateMVPAttrib(m_mesh_program);      // update uniforms
  //m_graphManager->update2Dflag(true, m_uniforms);
  //m_opengl_mngr->update2Dflag(true);
}

void GLWidget::mouseMoveEvent(QMouseEvent* event)
{
  int deltaX = event->x() - m_lastPos.x();
  int deltaY = event->y() - m_lastPos.y();

  if (event->modifiers() == Qt::ShiftModifier) {
    makeCurrent();

    m_opengl_mngr->renderSelection(&m_uniforms);

    int hvgxID = pickObject(event); // hovered object

    // mark it by lighter color
    m_opengl_mngr->highlightObject(hvgxID);

    doneCurrent();
  }
  else if (event->buttons() == Qt::RightButton) {
    m_translation = QVector3D(m_translation.x() + deltaX / (float)width(),
      m_translation.y() + -1.0 * (deltaY / (float)height()),
      0.0);

    setMouseTracking(false);

  }
  else if (event->buttons() == Qt::LeftButton /*m_xaxis < 60 || m_yaxis < 60*/) 
  {
    setMouseTracking(false);
    
    // Mouse release position - mouse press position
    QVector2D diff = QVector2D(deltaX, deltaY);
    
    // Rotation axis is perpendicular to the mouse position difference
    QVector3D n = QVector3D(diff.y(), diff.x(), 0).normalized();

    // Accelerate angular speed relative to the length of the mouse sweep
    qreal angle = diff.length() / 2.0;;

    // Calculate new rotation axis as weighted sum
    m_rotationAxis = (m_rotationAxis + n).normalized();
    
    // angle in degrees and rotation axis
    m_rotation = QQuaternion::fromAxisAndAngle(m_rotationAxis, angle) * m_rotation;

    // whenever the rotation matrix is changed, reset the force directed layout and the nodes rotation matrix!
    // the 2D view is always locked from rotating
    // and the reset is alway rotatable

    // do wait function, if the user stayed in this view more than t seconds then do this
    // 1) stop previous layouting algorithm if running

    /*if (m_FDL_running) {
      stopForecDirectedLayout();
    }*/

    m_lockRotation2D_timer->start(500);
  }
  else {
    setMouseTracking(false);
  }

  m_lastPos = event->pos();
  event->accept();

  update();

  doneCurrent();
}

void GLWidget::wheelEvent(QWheelEvent* event)
{
  int delta = event->delta();

  if (event->orientation() == Qt::Vertical) {
    if (delta < 0) {
      m_distance *= 1.1;
    }
    else {
      m_distance *= 0.9;
    }
    m_opengl_mngr->setZoom(m_distance);
    update();
  }
}

void GLWidget::keyPressEvent(QKeyEvent* event)
{
  switch (event->key()) {
  case(Qt::Key_S): // reset
    m_rotation.setScalar(1.0f);
    m_rotation.setX(0.0f);
    m_rotation.setY(0.0f);
    m_rotation.setZ(0.0f);
    //reset translation
    m_translation = QVector3D(0.0, 0.0, 0.0);
    //reset zoom
    m_distance = 1.0f;
    m_opengl_mngr->setZoom(m_distance);
    update();
    break;
  case(Qt::Key_X): // stop layouting algorithm
    stopForecDirectedLayout();
    break;
  case(Qt::Key_Shift): // enable hover
    setMouseTracking(true);
  case(Qt::Key_0): // switch vbo mesh rendering order
    m_opengl_mngr->renderOrderToggle();
    update();
    break;
  case(Qt::Key_P):
    saveScreenshot();
    break;
  case (Qt::Key_Down):
    if ((m_rot_ydiff - 0.1) > 0)
      m_rot_ydiff -= 0.1;
    else if ((m_rot_ydiff - 0.01) > 0)
      m_rot_ydiff -= 0.01;

    qDebug() << m_rot_ydiff;
    break;
  case (Qt::Key_Up):
    m_rot_ydiff += 0.1;
    qDebug() << m_rot_ydiff;
    break;
  case(Qt::Key_Right):
    if (m_xy_slice_z < MESH_MAX_Z) {
      m_xy_slice_z += 0.005f * MESH_MAX_Z;
    }
    break;
  case(Qt::Key_Left):
    if (m_xy_slice_z > 0.0f) {
      m_xy_slice_z -= 0.005f * MESH_MAX_Z;
    }
    break;
  }
  
}

void GLWidget::saveScreenshot()
{
  QString fileName = QFileDialog::getSaveFileName(this,
    tr("Save Screenshot"), "",
    tr("PNG Images (*.png);;All Files (*)"));

  if (!fileName.isEmpty())
  {
    QImage screenshot = grabFramebuffer();
    screenshot.save(fileName, "png");
  }
}

void GLWidget::getSliderX(int value)
{
  if (value > 100 || value < 0) {
    return;
  }

  if (value > 98)
    value = 100;

  m_xaxis = value;

  update();
}

void GLWidget::getSliderY(int value)
{
  if (value > 100 || value < 0) {
    return;
  }

  if (value > 98)
    value = 100;
  m_yaxis = value;

  update();
}

void GLWidget::getIntervalID(int ID)
{
  m_2dspace->updateID(ID);
}

void GLWidget::getActiveGraphTab(int tab_graph)
{
  m_active_graph_tab = tab_graph;
}

void GLWidget::reset_layouting(bool flag)
{
  if (m_FDL_running) {
    stopForecDirectedLayout();
  }

  m_lockRotation2D_timer->start(10);
}

//--------------------- Graph  Parameters ---------------------
//
void GLWidget::getGraphCr(double value)
{
  qDebug() << value << " getGraphCr " << m_active_graph_tab;
  struct FDR_param fdr_params = m_graphManager->getFDRParams(m_active_graph_tab);
  if (value != 0) fdr_params.Cr = value;
  m_graphManager->updateFDRParamts(m_active_graph_tab, fdr_params);
}

void GLWidget::getGraphCa(double value)
{
  qDebug() << value << " getGraphCa " << m_active_graph_tab;

  struct FDR_param fdr_params = m_graphManager->getFDRParams(m_active_graph_tab);
  if (value != 0) fdr_params.Ca = value;
  m_graphManager->updateFDRParamts(m_active_graph_tab, fdr_params);
}

void GLWidget::getGraphAABBdim(double value)
{
  qDebug() << value << " getGraphAABBdim " << m_active_graph_tab;

  struct FDR_param fdr_params = m_graphManager->getFDRParams(m_active_graph_tab);
  if (value != 0) fdr_params.AABBdim = value;
  m_graphManager->updateFDRParamts(m_active_graph_tab, fdr_params);
}

void GLWidget::getGraphmax_distance(double value)
{
  qDebug() << value << " getGraphmax_distance " << m_active_graph_tab;

  struct FDR_param fdr_params = m_graphManager->getFDRParams(m_active_graph_tab);
  if (value != 0) fdr_params.max_distance = value;
  m_graphManager->updateFDRParamts(m_active_graph_tab, fdr_params);
}

void GLWidget::getGraphmax_vertex_movement(double value)
{
  qDebug() << value << " getGraphmax_vertex_movement " << m_active_graph_tab;

  struct FDR_param fdr_params = m_graphManager->getFDRParams(m_active_graph_tab);
  if (value != 0) fdr_params.max_vertex_movement = value;
  m_graphManager->updateFDRParamts(m_active_graph_tab, fdr_params);
}

void GLWidget::getGraphslow_factor(double value)
{
  qDebug() << value << " getGraphslow_factor " << m_active_graph_tab;

  struct FDR_param fdr_params = m_graphManager->getFDRParams(m_active_graph_tab);
  if (value != 0) fdr_params.slow_factor = value;
  m_graphManager->updateFDRParamts(m_active_graph_tab, fdr_params);
}

void GLWidget::getGraphmax_force(double value)
{
  qDebug() << value << " getGraphmax_force " << m_active_graph_tab;

  struct FDR_param fdr_params = m_graphManager->getFDRParams(m_active_graph_tab);
  if (value != 0) fdr_params.max_force = value;
  m_graphManager->updateFDRParamts(m_active_graph_tab, fdr_params);
}

void GLWidget::getGraphoriginalPosAttraction(double value)
{
  qDebug() << value << " getGraphoriginalPosAttraction " << m_active_graph_tab;

  struct FDR_param fdr_params = m_graphManager->getFDRParams(m_active_graph_tab);
  if (value != 0) fdr_params.originalPosAttraction = value;
  m_graphManager->updateFDRParamts(m_active_graph_tab, fdr_params);
}

//------------------------------------------------------
//
void GLWidget::getFilteredID(QString value)
{
  if (m_opengl_mngr == NULL)
    return;
  // check if there are more than one ID
  QList<QString> tokens = value.split(',');
  qDebug() << tokens;

  stopForecDirectedLayout();

  bool invisibility = false; // meaning not filtered (visible)
  m_opengl_mngr->FilterByID(tokens, invisibility);

  // start force layout
  m_lockRotation2D_timer->start(0);
  //  check whatever needed to be updated in the checkbox
  std::map<Object_t, std::pair<int, int>> visibilityUpdate = m_opengl_mngr->getObjectCountByType();
  this->getToggleCheckBox(visibilityUpdate);

  update();
}

//------------------------------------------------------
//
void GLWidget::stopForecDirectedLayout()
{
  // stop force layout
//    m_refresh_timer->stop();
  for (int i = 0; i < max_graphs; ++i)
    m_graphManager->stopForceDirectedLayout(i);

  m_FDL_running = false;
}

//------------------------------------------------------
//
void GLWidget::getFilterWithChildren(bool value)
{
  // update this in openglmanager
  m_opengl_mngr->updateDisplayChildFlag(value);
}

//------------------------------------------------------
//
void GLWidget::getFilterWithParent(bool value)
{
  m_opengl_mngr->updateDisplayParentFlag(value);
}

//------------------------------------------------------
//
void GLWidget::getFilterWithSynapses(bool value)
{
  m_opengl_mngr->updateDisplaySynapseFlag(value);
}

//------------------------------------------------------
//
void GLWidget::getDepth(int d)
{
  if (d < 0)
    return;

  m_opengl_mngr->updateDepth(d);
}

//------------------------------------------------------
//
void GLWidget::getNodeSizeEncoding(QString encoding)
{
  qDebug() << encoding;
  if (encoding == "Volume")
    m_opengl_mngr->updateNodeSizeEncoding(Size_e::VOLUME);
  else if (encoding == "Astrocyte Coverage")
    m_opengl_mngr->updateNodeSizeEncoding(Size_e::ASTRO_COVERAGE);
  else if (encoding == "Synapse Size")
    m_opengl_mngr->updateNodeSizeEncoding(Size_e::SYNAPSE_SIZE);
  update();
}

//------------------------------------------------------
//
void GLWidget::getColorEncoding(QString encoding)
{
  qDebug() << encoding;
  if (encoding == "Type")
    m_opengl_mngr->updateColorEncoding(Color_e::TYPE);
  else if (encoding == "Astrocyte Coverage")
    m_opengl_mngr->updateColorEncoding(Color_e::ASTRO_COVERAGE);
  else if (encoding == "Function")
    m_opengl_mngr->updateColorEncoding(Color_e::FUNCTION);
  else if (encoding == "Glycogen Distribution")
    m_opengl_mngr->updateColorEncoding(Color_e::GLYCOGEN_MAPPING);
  update();
}

//------------------------------------------------------
//
void GLWidget::get2DtextureEncoding(QString encoding)
{
  qDebug() << encoding;
  if (encoding == "Astrocyte Coverage")
    m_opengl_mngr->update2DTextureEncoding(HeatMap2D_e::ASTRO_COVERAGE);
  else if (encoding == "Glycogen Distribution")
    m_opengl_mngr->update2DTextureEncoding(HeatMap2D_e::GLYCOGEN_MAPPING);
  update();
}


//------------------------------------------------------
//
void GLWidget::getItemChanged(QListWidgetItem* item)
{
  Qt::CheckState state = item->checkState();
  bool flag;
  if (state == Qt::Unchecked) // do filter them from view
    flag = true;
  else if (state == Qt::Checked) {
    flag = false;
  }
  else {
    return;
  }


  getFilteredType(item->text(), flag);

  update();
}

//------------------------------------------------------
//
void GLWidget::getFilteredType(QString value, bool flag)
{
  if (m_opengl_mngr == NULL)
    return;

  stopForecDirectedLayout();

  Object_t object_type = Object_t::UNKNOWN;
  if (value == "Axons")
    object_type = Object_t::AXON;
  else if (value == "Dendrites")
    object_type = Object_t::DENDRITE;
  else if (value == "Boutons")
    object_type = Object_t::BOUTON;
  else if (value == "Spines")
    object_type = Object_t::SPINE;
  else if (value == "Mitochondria")
    object_type = Object_t::MITO;
  else if (value == "Synapse")
    object_type = Object_t::SYNAPSE;
  else if (value == "Astrocyte")
    object_type = Object_t::ASTROCYTE;

  m_opengl_mngr->FilterByType(object_type, flag);

  // start force layout
  m_lockRotation2D_timer->start(0);

  update();
}

//------------------------------------------------------
//
void GLWidget::getDoubleClickedTableView(QModelIndex index)
{
  RemoveRowAt_GL(index);
}

//------------------------------------------------------
//
void GLWidget::getDeletedHVGXID(int hvgxID)
{
  m_selectedObjects.erase(hvgxID);
}

//------------------------------------------------------
//
void GLWidget::getFitlerButtonClicked(bool)
{
  bool invisibility = false; // meaning not filtered (visible)
  m_opengl_mngr->FilterByID(m_selectedObjects, invisibility);
  //  check whatever needed to be updated in the checkbox
  std::map<Object_t, std::pair<int, int>> visibilityUpdate = m_opengl_mngr->getObjectCountByType();
  this->getToggleCheckBox(visibilityUpdate);

  update();
}

//------------------------------------------------------
//
void GLWidget::getResetFitlerButtonClicked(bool)
{
  m_opengl_mngr->showAll();
  checkAllListWidget_GL();
  update();
}

//------------------------------------------------------
//
void GLWidget::clearSelectedObjectsTable()
{
  clearRowsTable();
  m_selectedObjects.clear();
}

//------------------------------------------------------
//
void GLWidget::hideSelectedObjects()
{
  m_hide_toggle = !m_hide_toggle;
  m_opengl_mngr->VisibilityToggleSelectedObjects(m_selectedObjects, m_hide_toggle);
  //  check whatever needed to be updated in the checkbox
  std::map<Object_t, std::pair<int, int>> visibilityUpdate = m_opengl_mngr->getObjectCountByType();
  this->getToggleCheckBox(visibilityUpdate);
  update();
}

void GLWidget::highlightSelected(QModelIndex index)
{
  GetIDFrom(index);
}

void GLWidget::getHVGXIDAtSelectedRow(int ID)
{
  m_opengl_mngr->highlightObject(ID);

}

void GLWidget::togglWeightedAstroCoverage()
{
  m_opengl_mngr->toggleWeightedCoverage();
}

void GLWidget::getglycogenMappedSelectedState(QString ID_str, bool state)
{
  int ID = ID_str.toInt();
  m_opengl_mngr->highlightObject(ID);
  std::set<int> obj_set;
  obj_set.insert(ID);
  m_opengl_mngr->VisibilityToggleSelectedObjects(obj_set, !state);
  //  check whatever needed to be updated in the checkbox
  std::map<Object_t, std::pair<int, int>> visibilityUpdate = m_opengl_mngr->getObjectCountByType();
  this->getToggleCheckBox(visibilityUpdate);
}

void GLWidget::getProximityTypeState(QString type, bool flag)
{
  int flag_value = (flag) ? 1 : 0;
  if (type == "Astrocyte") {
    m_filterByProximityType.setX(flag_value);
  }
  else if (type == "Glycogen") {
    m_filterByProximityType.setY(flag_value);
  }
  else if (type == "Astro. Mitochondria") {
    m_filterByProximityType.setZ(flag_value);
  }
}

void GLWidget::getFilteredListByProximity()
{
  m_opengl_mngr->updateFilterByProximityType(m_filterByProximityType);

  qDebug() << "Insert in selected Objects";
  std::vector<int> ssbo_filtered_by_proximity = m_opengl_mngr->getSSBOFilterByProximity();

  for (int i = 0; i < ssbo_filtered_by_proximity.size(); ++i) {
    int hvgxID = ssbo_filtered_by_proximity[i];
    if (m_selectedObjects.find(hvgxID) == m_selectedObjects.end()) {
      m_selectedObjects.insert(hvgxID);
      insertInTable(hvgxID);
    }
  }
}

void GLWidget::updateMinProximity(double min)
{
  m_opengl_mngr->updateMinProximity(min);
}

void GLWidget::getToggleCheckBox(std::map<Object_t, std::pair<int, int>> visibilityUpdate)
{
  std::map<QString, int> checkBoxByType;
  for (auto iter = visibilityUpdate.begin(); iter != visibilityUpdate.end(); iter++) {
    Object_t obj_type = (*iter).first;
    std::pair<int, int> counts_pair = (*iter).second;

    QString obj_typeName;
    int flag;

    switch (obj_type) {
    case Object_t::ASTROCYTE: // 0
      obj_typeName = "Astrocyte";
      break;
    case Object_t::SPINE:
      obj_typeName = "Spines";
      break;
    case Object_t::DENDRITE:
      obj_typeName = "Dendrites";
      break;
    case Object_t::AXON:
      obj_typeName = "Axons";
      break;
    case Object_t::BOUTON:
      obj_typeName = "Boutons";
      break;
    case Object_t::MITO:
      obj_typeName = "Mitochondria";
      break;
    case Object_t::SYNAPSE:
      obj_typeName = "Synapse";
      break;
    }

    if (counts_pair.second <= 0)
      flag = 0; // not checked
    else if (counts_pair.second == counts_pair.first)
      flag = 1; // full
    else
      flag = 2; // half


    checkBoxByType[obj_typeName] = flag;
  }

  signalCheckByType(checkBoxByType);

}

void GLWidget::drawScene()
{
  m_mesh_vao.bind();
 
  m_shared_resources.mesh_index_vbo->bind();
  glDrawElements(GL_TRIANGLES, m_shared_resources.index_count, GL_UNSIGNED_INT, 0);

  m_mesh_vao.release();
}

void GLWidget::initVisibilitySSBO()
{ 
  setVisibleStructures();

  int bufferSize = m_visible_structures.size() * sizeof(int);

  glGenBuffers(1, &m_visibility_ssbo);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_visibility_ssbo);
  glBufferData(GL_SHADER_STORAGE_BUFFER, bufferSize, m_visible_structures.data(), GL_DYNAMIC_COPY);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, m_visibility_ssbo);
  qDebug() << "m_ssbo_data buffer size: " << bufferSize;
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

  GL_Error();

  //writeVisibilitySSBO();

}

void GLWidget::setVisibleStructures()
{
  std::map<int, Object*>* objects_map = m_data_containter->getObjectsMapPtr();

  //iterate over all objects and add indices to the VBO
  for (auto iter = objects_map->rbegin(); iter != objects_map->rend(); iter++)
  {
    Object* object_p = (*iter).second;
    int currentID = object_p->getHVGXID();

    if (m_is_overview_widget || currentID == m_selected_hvgx_id || objects_map->at(m_selected_hvgx_id)->isChild(currentID) || objects_map->at(m_selected_hvgx_id)->isParent(currentID))
    {
      m_visible_structures.push_back(currentID);
    }
  }
}

void GLWidget::writeVisibilitySSBO()
{
  int bufferSize = m_visible_structures.size() * sizeof(int);

  glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_visibility_ssbo);
  GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
  memcpy(p, m_visible_structures.data(), bufferSize);
  glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
}

void GLWidget::prepareResize()
{
  makeCurrent();
  
  // make sure memory is deallocated before fbo is resized
  if (m_init)
  {
    glDeleteBuffers(2, oit_buffers);
    glDeleteTextures(1, &headPtrTex);
    glDeleteBuffers(1, &clear_oit_buffers);
  }
}

void GLWidget::pass1()
{
  glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &mesh_shader_pass_idx_1);
  glClear(GL_COLOR_BUFFER_BIT);
  glDepthMask(GL_FALSE);
  drawScene();
  glFinish();
}

void GLWidget::pass2()
{
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
  glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &mesh_shader_pass_idx_2);
  glClear(GL_COLOR_BUFFER_BIT);

  QMatrix4x4 mat = QMatrix4x4();
  mat.setToIdentity();

  int mMatrix = m_mesh_program->uniformLocation("mMatrix");
  if (mMatrix >= 0) m_mesh_program->setUniformValue(mMatrix, mat);

  int vMatrix = m_mesh_program->uniformLocation("vMatrix");
  if (vMatrix >= 0) m_mesh_program->setUniformValue(vMatrix, mat);

  int pMatrix = m_mesh_program->uniformLocation("pMatrix");
  if (pMatrix >= 0) m_mesh_program->setUniformValue(pMatrix, mat);

  // Draw a screen filler
  m_fsQuad_vao.bind();
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
  m_fsQuad_vao.release();
}

void GLWidget::clearBuffers()
{
  GLuint zero = 0;
  glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, oit_buffers[COUNTER_BUFFER]);
  glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &zero);

  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, clear_oit_buffers);
  glBindTexture(GL_TEXTURE_2D, headPtrTex);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
}

void GLWidget::initMeshShaderStorage(int width, int height)
{
  m_maxNodes = 20 * width * height;
  GLuint nodeSize = 5 * sizeof(GLfloat) + sizeof(GLuint); // The size of a linked list node

  glGenBuffers(2, oit_buffers);

  // Our atomic counter
  glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, oit_buffers[COUNTER_BUFFER]);
  glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);

  // The buffer of linked lists
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, oit_buffers[LINKED_LIST_BUFFER]);
  glBufferData(GL_SHADER_STORAGE_BUFFER, (GLsizeiptr)(m_maxNodes * nodeSize), NULL, GL_DYNAMIC_DRAW);

  // The buffer for the head pointers, as an image texture
  glGenTextures(1, &headPtrTex);
  glBindTexture(GL_TEXTURE_2D, headPtrTex);
  glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32UI, width, height);
  glBindImageTexture(0, headPtrTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);

  headPtrClearBuf->clear();
  headPtrClearBuf->shrink_to_fit();
  headPtrClearBuf = new std::vector<GLuint>(width * height, 0xffffffff);

  glGenBuffers(1, &clear_oit_buffers);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, clear_oit_buffers);
  glBufferData(GL_PIXEL_UNPACK_BUFFER, headPtrClearBuf->size() * sizeof(GLuint), headPtrClearBuf->data(), GL_STATIC_COPY);
  
  m_init = true;
}


// GET THE WIDGET LIST FROM MAIN WINDOW
// CALL ANOTHER FUNCTINO TO HANDLE THAT

// mapping and fitlering
// take the whole mapping
// iterate over the objects
// get their type
// check if they should be checked or not
void GLWidget::getMappingTreeWidget(QTreeWidget* treeWidget)
{
  qDebug() << "getListWidget" << treeWidget->topLevelItemCount();
  std::map<int, Object*>* objectMap = m_data_containter->getObjectsMapPtr();

  // iterate over these
  // use ID to get type of object
  // for each check in the visibilityMapByType

  QTreeWidgetItemIterator it(treeWidget);
  while (*it) {
    QString ID_str = ((*it)->text(0));
    int ID = ID_str.toInt();
    if (objectMap->find(ID) != objectMap->end()) {
      Object* obj = objectMap->at(ID);
      qDebug() << obj->getName().data();
      if (obj->isFiltered())
        (*it)->setCheckState(0, Qt::Unchecked);
      else
        (*it)->setCheckState(0, Qt::Checked);
    }
    ++it;
  }

}

void GLWidget::selectAllVisible()
{
  std::map<int, Object*>* objectMap = m_data_containter->getObjectsMapPtr();
  for (auto iter = objectMap->rbegin(); iter != objectMap->rend(); iter++) {
    Object* obj = (*iter).second;
    if (obj->isFiltered())
      continue;
    // add it to list
    int hvgxID = obj->getHVGXID();
    if (m_selectedObjects.find(hvgxID) == m_selectedObjects.end()) {
      m_selectedObjects.insert(hvgxID);
      insertInTable(hvgxID);
    }
  }
}

void GLWidget::updateProximitySSBO()
{
  m_opengl_mngr->updateProximitySSBOFlag();
}

void GLWidget::autoRotation()
{
  // only enabled if we are not in the 2D space

  m_auto_rotate = !m_auto_rotate;
}

void GLWidget::startRotation()
{
  // Mouse release position - mouse press position
  if (m_auto_rotate) {
    QVector2D diff = QVector2D(0, m_rot_ydiff);
    QVector3D n = QVector3D(diff.x(), diff.y() /* diff.y()*/, 0).normalized();

    // Accelerate angular speed relative to the length of the mouse sweep
    qreal acc = diff.length() / 2.0;;

    // Calculate new rotation axis as weighted sum
    m_rotationAxis = (m_rotationAxis + n).normalized();
    // angle in degrees and rotation axis
    m_rotation = QQuaternion::fromAxisAndAngle(m_rotationAxis, acc) * m_rotation;
  }
}
