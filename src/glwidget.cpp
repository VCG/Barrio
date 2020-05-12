/*
 * FinalMatrix = Projection * View * Model
 * Model = RotationAroundOrigin * TranslationFromOrigin * RotationAroundObjectCenter
 */
#include <QResource>
#include <QTimer>

#include "glwidget.h"
#include "colors.h"

GLWidget::GLWidget(QWidget* parent)
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

  m_active_graph_tab = 0;
  setFocusPolicy(Qt::StrongFocus);
  m_hide_toggle = false;

  m_auto_rotate = false;
  m_rot_ydiff = 1;

  m_xy_slice_z = 0.5f * MESH_MAX_Z;
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

  //glycogen analysis manager with clustering
  m_glycogenAnalysisManager = new GlycogenAnalysisManager(m_data_containter->getGlycogenMapPtr(), m_data_containter->getGlycogenVertexDataPtr(), m_data_containter->getGlycogenOctree(), m_data_containter->getMeshPointer()->getVerticesList());
  m_glycogenAnalysisManager->setBoutonAndSpineOctrees(m_data_containter->getBoutonHash(), m_data_containter->getSpineHash());
  m_glycogenAnalysisManager->setMitochondriaSpatialHash(m_data_containter->getNeuroMitoHash());

}

void GLWidget::updateMVPAttrib()
{
  // calculate model view transformation
  // world/model matrix: determines the position and orientation of an object in 3D space
  m_mMatrix.setToIdentity();

  // Center Zoom
  m_mMatrix.translate(m_center);
  m_mMatrix.scale(m_distance);
  m_mMatrix.translate(-1.0 * m_center);

  // Translation
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

  const qreal retinaScale = devicePixelRatio();
  QVector4D viewport = QVector4D(0, 0, width() * retinaScale, height() * retinaScale);

  int max_astro_coverage = m_data_containter->getMaxAstroCoverage();
  int max_volume = m_data_containter->getMaxVolume();
  // graph model matrix without rotation, apply rotation to nodes directly
  m_uniforms = { m_yaxis, m_xaxis, m_mMatrix.data(), m_vMatrix.data(), m_projection.data(),
                      m_model_noRotation.data(), m_rotationMatrix, viewport, max_volume, max_astro_coverage, 0.0001, m_xy_slice_z};
  m_opengl_mngr->updateUniformsData(m_uniforms);
}

void GLWidget::initializeGL()
{
  qDebug() << "initializeGL";
  m_performaceMeasure.startTimer();

  initializeOpenGLFunctions();

  updateMVPAttrib();


  m_2dspace->initOpenGLFunctions();
  m_opengl_mngr->initOpenGLFunctions();
  //m_graphManager->ExtractGraphFromMesh();



  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /******************** 1 Abstraction Space ********************/
  m_2dspace->initBuffer();
  emit setAbstractionData(m_2dspace);

  glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
  glEnable(GL_MULTISAMPLE);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND); // to enable transparency
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_LINE_SMOOTH); // for line smoothing
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

  //GLuint meshShaderProgramHandle = m_opengl_mngr->getMeshShaderProgramHandle();
  //initMeshShaderStorage(meshShaderProgramHandle);

  //mesh_shader_pass_idx_1 = glGetSubroutineIndex(meshShaderProgramHandle, GL_FRAGMENT_SHADER, "pass1");
  //mesh_shader_pass_idx_2 = glGetSubroutineIndex(meshShaderProgramHandle, GL_FRAGMENT_SHADER, "pass2");


  //// Set up a  VAO for the full-screen quad
  //GLfloat verts[] = { -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f,
  //  1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f };
  //GLuint bufHandle;
  //glGenBuffers(1, &bufHandle);
  //glBindBuffer(GL_ARRAY_BUFFER, bufHandle);
  //glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(GLfloat), verts, GL_STATIC_DRAW);

  //// Set up the vertex array object
  //glGenVertexArrays(1, &fsQuad);
  //glBindVertexArray(fsQuad);

  //glBindBuffer(GL_ARRAY_BUFFER, bufHandle);
  //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  //glEnableVertexAttribArray(0);  // Vertex position

  //glBindVertexArray(0);



  if (m_FDL_running) {
    stopForecDirectedLayout();
  }

  m_lockRotation2D_timer->start(500);

}

void GLWidget::pass1()
{
  float fps = m_performaceMeasure.getFPS();
  if (fps > 0) {
    updateFPS(QString::number(fps));
    updateFrameTime(QString::number(1000.0 / fps));
  }

  startRotation();

  //glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &mesh_shader_pass_idx_1);
  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDepthMask(GL_FALSE);
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  const qreal retinaScale = devicePixelRatio();
  glViewport(0, 0, width() * retinaScale, height() * retinaScale);
  updateMVPAttrib();
  
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  //m_opengl_mngr->updateUniformsData(m_uniforms);
  //m_opengl_mngr->drawAll();

  // draw scene
  m_opengl_mngr->drawMeshTriangles(false, &m_uniforms);

  glFinish();
}

void GLWidget::pass2()
{
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
  glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &mesh_shader_pass_idx_2);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //view = glm::mat4(1.0f);
  //projection = glm::mat4(1.0f);
  //model = glm::mat4(1.0f);

  //setMatrices();

  // Draw a screen filler
  glBindVertexArray(fsQuad);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
  glBindVertexArray(0);
}

void GLWidget::clearBuffers()
{
  GLuint zero = 0;
  glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, oit_buffers[COUNTER_BUFFER]);
  glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &zero);

  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, clear_oit_buffers);
  glBindTexture(GL_TEXTURE_2D, headPtrTex);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width(), height(), GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
}


void GLWidget::paintGL()
{
  //clearBuffers();
  //pass1();
  //pass2();
  

  float fps = m_performaceMeasure.getFPS();
  if (fps > 0) {
    updateFPS(QString::number(fps));
    updateFrameTime(QString::number(1000.0 / fps));
  }

  startRotation();

  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  const qreal retinaScale = devicePixelRatio();
  glViewport(0, 0, width() * retinaScale, height() * retinaScale);


  updateMVPAttrib();
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  //m_opengl_mngr->updateUniformsData(m_uniforms);
  m_opengl_mngr->drawCoordSystem(&m_uniforms);
  //m_opengl_mngr->drawSlice(&m_uniforms);
  m_opengl_mngr->drawMeshTriangles(false, &m_uniforms);
  

  // paint the text here
 
}

void GLWidget::resizeGL(int w, int h)
{
  // Calculate aspect ratio
  const qreal retinaScale = devicePixelRatio();
  h = (h == 0) ? 1 : h;
  glViewport(0, 0, w * retinaScale, h * retinaScale);

  //qDebug() << w * retinaScale << " " << h * retinaScale;
  m_uniforms.viewport = QVector4D(0, 0, w * retinaScale, h * retinaScale);

  qreal aspect = retinaScale * qreal(w) / qreal(h ? h : 1);
  m_projection.setToIdentity();
  //m_projection.ortho(GLfloat(-w) / GLfloat(h), GLfloat(w) / GLfloat(h), -1.0, 1.0f, -5.0, 5.0);
  m_projection.perspective(45.0, aspect, 1.0, 10 * MESH_MAX_Z);

  // set up view
  // view matrix: transform a model's vertices from world space to view space, represents the camera
  m_center = QVector3D(MESH_MAX_X / 2.0, MESH_MAX_Y / 2.0, MESH_MAX_Z / 2.0);
  QVector3D  cameraUpDirection = QVector3D(0.0, 1.0, 0.0);
  QVector3D eye = QVector3D(MESH_MAX_X / 2.0, MESH_MAX_Y / 2.0, 2.0 * MESH_MAX_Z);
  
  m_vMatrix.setToIdentity();
  m_vMatrix.lookAt(eye, m_center, cameraUpDirection);

  if (m_opengl_mngr != NULL)
    m_opengl_mngr->updateCanvasDim(w, h, retinaScale);
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

  m_opengl_mngr->renderSelection(&m_uniforms);

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
  }

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
  updateMVPAttrib();      // update uniforms
  //m_graphManager->update2Dflag(true, m_uniforms);
  m_opengl_mngr->update2Dflag(true);
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

void GLWidget::drawMesh()
{

}

void GLWidget::initMeshShaderStorage(GLuint program)
{
  glGenBuffers(2, oit_buffers);
  GLint maxNodes = 20 * width() * height();
  GLint nodeSize = 5 * sizeof(GLfloat) + sizeof(GLuint); // The size of a linked list node

  // Our atomic counter
  glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, oit_buffers[COUNTER_BUFFER]);
  glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);

  // The buffer for the head pointers, as an image texture
  glGenTextures(1, &headPtrTex);
  glBindTexture(GL_TEXTURE_2D, headPtrTex);
  glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32UI, width(), height());
  glBindImageTexture(0, headPtrTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);

  // The buffer of linked lists
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, oit_buffers[LINKED_LIST_BUFFER]);
  glBufferData(GL_SHADER_STORAGE_BUFFER, maxNodes * nodeSize, NULL, GL_DYNAMIC_DRAW);

  // set max nodes uniform
  int maxNodesID = glGetUniformLocation(program, "maxNodes");
  if (maxNodesID >= 0) glUniform1iv(maxNodesID, 1, &maxNodes);

  std::vector<GLuint> headPtrClearBuf(width() * height(), 0xffffffff);
  glGenBuffers(1, &clear_oit_buffers);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, clear_oit_buffers);
  glBufferData(GL_PIXEL_UNPACK_BUFFER, headPtrClearBuf.size() * sizeof(GLuint), &headPtrClearBuf[0], GL_STATIC_COPY);
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
