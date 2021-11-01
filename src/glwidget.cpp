#include <QResource>
#include <QTimer>

#include "glwidget.h"
#include "colors.h"
#include "mainwidget.h"

GLWidget::GLWidget(int hvgx_id, SharedGLResources* resources, bool isOverviewWidget, QWidget* parent)
	: QOpenGLWidget(parent)
{
	m_distance_threshold = resources->distance_threshold;
	m_all_selected_mitos = resources->all_selected_mitos;

	connect(this, SIGNAL(aboutToResize()), this, SLOT(prepareResize()));

	this->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ShowContextMenu(const QPoint&)));

	m_parent = parent;

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
}

void GLWidget::init(DataContainer* data_container, bool use_camera_settings, CameraSettings cameraSettings)
{
	m_data_container = data_container;
	QVector3D center;
	Object* object = m_data_container->getObject(m_selected_hvgx_id);
	if (object != nullptr)
	{
		center = object->getCenter().toVector3D();
		m_parent_id = object->getParentID();
	}
	else
	{
		m_parent_id = 0;
	}

	if (m_is_overview_widget)
	{
		center = QVector3D(MESH_MAX_X / 2.0, MESH_MAX_Y / 2.0, MESH_MAX_Z / 2.0);
	}

	float fov = 45.0f;
	float nearPlane = 1.0f;
	float farPlane = 50.0f;

	if (use_camera_settings)
	{
		camera = new Camera(cameraSettings, fov, nearPlane, farPlane);
	}
	else
	{
		camera = new Camera(fov, nearPlane, farPlane, center);
	}

}

void GLWidget::updateMVPAttrib(QOpenGLShaderProgram* program)
{
	int mMatrix = program->uniformLocation("mMatrix");
	if (mMatrix >= 0) program->setUniformValue(mMatrix, camera->getModelMatrix());

	int vMatrix = program->uniformLocation("vMatrix");
	if (vMatrix >= 0) program->setUniformValue(vMatrix, camera->getViewMatrix());

	int pMatrix = program->uniformLocation("pMatrix");
	if (pMatrix >= 0) program->setUniformValue(pMatrix, camera->getProjectionMatrix());

	int eye = program->uniformLocation("eye");
	if (eye >= 0) program->setUniformValue(eye, camera->getPosition());

	int mNodes = program->uniformLocation("maxNodes");
	if (mNodes >= 0) program->setUniformValue(mNodes, m_maxNodes);

	int show_mcd_colormap = program->uniformLocation("color_code");
	if (show_mcd_colormap >= 0) program->setUniformValue(show_mcd_colormap, m_shared_resources->show_color_coding);

	int show_silhouette = program->uniformLocation("show_silhouette");
	if (show_silhouette >= 0) program->setUniformValue(show_silhouette, m_shared_resources->show_silhouette);

	int main_mito = program->uniformLocation("main_mito");
	if (main_mito >= 0) program->setUniformValue(main_mito, m_selected_hvgx_id);

	int showSlice = program->uniformLocation("showSlice");
	if (showSlice >= 0) program->setUniformValue(showSlice, m_shared_resources->show_slice);

	int show3D = program->uniformLocation("show3D");
	if (show3D >= 0) program->setUniformValue(show3D, m_shared_resources->show_3d_renderings);

	int zSlice = program->uniformLocation("slice_z");
	if (zSlice >= 0) program->setUniformValue(zSlice, m_shared_resources->slice_depth);

	int is_overview = program->uniformLocation("is_overview");
	if (is_overview >= 0) program->setUniformValue(is_overview, m_is_overview_widget);

	int curr_hovered = program->uniformLocation("currently_hovered_id");
	if (curr_hovered >= 0) program->setUniformValue(curr_hovered, m_shared_resources->currently_hovered_widget);

	GL_Error();
}

void GLWidget::updateVisParameters()
{
	makeCurrent();
	m_mesh_program->bind();

	int opacity = m_mesh_program->uniformLocation("cell_opacity");
	if (opacity >= 0) m_mesh_program->setUniformValue(opacity, m_shared_resources->cell_opacity);

	update();
	m_mesh_program->release();
}

void GLWidget::resetCamera()
{
	// todo
}

void GLWidget::zoom(double delta)
{
	camera->processScroll(delta);
	update();
}

void GLWidget::initializeGL()
{
	initializeOpenGLFunctions();

	QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();

	f->glClearColor(1.0, 1.0, 1.0, 1.0);

	m_mesh_program = new QOpenGLShaderProgram;
	m_mesh_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/mesh_vert_simplified.glsl");
	m_mesh_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/mesh_frag_simplified.glsl");
	m_mesh_program->link();

	f->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int success = m_mesh_program->bind();

	m_mesh_vao.create();
	m_mesh_vao.bind();

	// bind vbos to vao
	m_shared_resources->vertex_vbo->bind();

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

	// normal
	offset += sizeof(GL_INT);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);

	// is skeleton
	offset += sizeof(QVector4D);
	glEnableVertexAttribArray(5);
	glVertexAttribIPointer(5, 1, GL_INT, stride, (GLvoid*)offset);

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

	updateVisibilitySSBO();

	int mito_colormap = m_mesh_program->uniformLocation("mito_colormap");
	if (mito_colormap >= 0)
	{
		glUniform1i(mito_colormap, 1);
	}

	int volumePos = m_mesh_program->uniformLocation("volume");
	if (volumePos >= 0)
	{
		glUniform1i(volumePos, 0);
	}

	m_mesh_program->release();
	updateVisParameters();
	//m_lockRotation2D_timer->start(500);
}

void GLWidget::paintGL()
{
	camera->frameUpdate();

	m_mesh_program->bind();
	updateMVPAttrib(m_mesh_program);
	updateHighlightedSSBO();

	clearBuffers();
	pass1();
	glFlush();
	pass2();

	m_mesh_program->release();

	// overview widget needs to update every frame
	if (m_is_overview_widget)
	{
		update();
	}
}

void GLWidget::resizeGL(int w, int h)
{
	const qreal retinaScale = devicePixelRatio();
	h = (h == 0) ? 1 : h;

	m_width = w * retinaScale;
	m_height = h * retinaScale;

	qreal aspect_ratio = retinaScale * qreal(w) / qreal(h ? h : 1);

	camera->setAspectRatio(aspect_ratio);

	initMeshShaderStorage(m_width, m_height);
}

/*int GLWidget::pickObject(QMouseEvent* event)
{
	const qreal retinaScale = devicePixelRatio();
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	int x = event->x() * retinaScale;
	int y = viewport[3] - event->y() * retinaScale;
	int hvgxID = processSelection(x, y);
	if (hvgxID == 16777215)
	{
		hvgxID = 0;
	}

	qDebug() << "Selected ID: " << hvgxID;
	setHoveredID(hvgxID);
	std::string name = m_data_container->getObjectName(hvgxID);
	QString oname = QString::fromUtf8(name.c_str());
	setHoveredName(oname);
	return hvgxID;
}*/

void GLWidget::enterEvent(QEvent* event)
{
	m_shared_resources->currently_hovered_widget = m_parent_id;
	update();
}
void GLWidget::leaveEvent(QEvent* event)
{
	m_shared_resources->currently_hovered_widget = 0;
	update();
}

void GLWidget::mousePressEvent(QMouseEvent* event)
{
	camera->mouse_press_event(event);
	makeCurrent();

quit:
	doneCurrent();

	event->accept();
}

void GLWidget::mouseMoveEvent(QMouseEvent* event)
{
	camera->mouse_move_event(event);
	update();
	doneCurrent();
}

void GLWidget::wheelEvent(QWheelEvent* event)
{
	int delta = event->delta();

	if (event->orientation() == Qt::Vertical)
	{
		camera->processScroll(delta);
		update();
	}
}

void GLWidget::keyPressEvent(QKeyEvent* event)
{
	switch (event->key()) {
	case(Qt::Key_S): // reset
		break;
	case(Qt::Key_X): // stop layouting algorithm
	  //stopForecDirectedLayout();
		break;
	case(Qt::Key_Shift): // enable hover
		setMouseTracking(true);
	case(Qt::Key_0): // switch vbo mesh rendering order
		break;
	}
}

void GLWidget::getSliderX(int value)
{
	if (value > 100 || value < 0) {
		return;
	}

	if (value > 98)
		value = 100;
}

void GLWidget::getSliderY(int value)
{
	if (value > 100 || value < 0) {
		return;
	}

	if (value > 98)
		value = 100;
}

void GLWidget::getIntervalID(int ID)
{

}

void GLWidget::drawScene()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, *m_shared_resources->image_stack_volume);

	glActiveTexture(GL_TEXTURE0 + 1); // activate the texture unit first before binding texture
	glBindTexture(GL_TEXTURE_1D, *m_shared_resources->mito_cell_distance_colormap);

	m_mesh_vao.bind();

	m_shared_resources->mesh_index_vbo->bind();
	glDrawElements(GL_TRIANGLES, m_shared_resources->mesh_index_count, GL_UNSIGNED_INT, 0);

	m_mesh_vao.release();
}

void GLWidget::updateHighlightedSSBO()
{
	int bufferSize = m_shared_resources->highlighted_objects->size() * sizeof(int);

	glGenBuffers(1, &m_highlighted_ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_highlighted_ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, bufferSize, m_shared_resources->highlighted_objects->data(), GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, m_highlighted_ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void GLWidget::updateVisibilitySSBO()
{
	makeCurrent();
	if (!m_is_overview_widget)
	{
		setVisibleStructuresSingelObject();
	}
	else {
		setVisibleStructuresOverView();
	}

	int bufferSize = m_visible_structures.size() * sizeof(int);

	glGenBuffers(1, &m_visibility_ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_visibility_ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, bufferSize, m_visible_structures.data(), GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, m_visibility_ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	update();
}

void GLWidget::setVisibleStructuresSingelObject()
{
	m_visible_structures.clear();
	setVisibleStructures(m_selected_hvgx_id);
}

void GLWidget::setVisibleStructuresOverView()
{
	m_visible_structures.clear();
	for (auto id : *m_all_selected_mitos)
	{
		setVisibleStructures(id);
	}
}

void GLWidget::setVisibleStructures(int id)
{
	std::map<int, Object*> objects_map = m_data_container->getObjectsMap();

	Object* object = objects_map.at(id);
	int parentID = object->getParentID();

	m_visible_structures.push_back(parentID);

	Object* parent = objects_map.at(parentID);

	if (m_shared_resources->show_related_synapses)
	{
		std::vector<Object*>* synapses = parent->getSynapses();
		for (int i = 0; i < synapses->size(); i++)
		{
			m_visible_structures.push_back(synapses->at(i)->getHVGXID());
		}
	}
	else
	{
		std::vector<Object*> synapses = m_data_container->getObjectsByType(Object_t::SYNAPSE);

		for (auto syn : synapses)
		{
			std::map<int, double>* distance_map = objects_map.at(id)->get_distance_map_ptr();
			double distance = distance_map->at(syn->getHVGXID());
			if (distance < m_distance_threshold)
			{
				m_visible_structures.push_back(syn->getHVGXID());
			}
		}
	}

	std::vector<int>* siblings = parent->getChildrenIDs();
	for (size_t i = 0; i < siblings->size(); i++)
	{
		m_visible_structures.push_back(siblings->at(i));
	}
}

CameraSettings GLWidget::getCameraSettings()
{
	return camera->getCameraSettings();
}

void GLWidget::update_synapse_distance_threshold(double distance)
{
	m_distance_threshold = distance;
	updateVisibilitySSBO();
}

void GLWidget::update_visibility()
{
	updateVisibilitySSBO();
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

void GLWidget::ShowContextMenu(const QPoint& pos)
{
	QMenu contextMenu(tr("Context menu"), this);
	QAction action1("Remove Widget", this);

	connect(&action1, SIGNAL(triggered()), m_parent, SLOT(OnWidgetClose()));
	contextMenu.addAction(&action1);

	contextMenu.exec(mapToGlobal(pos));

}

void GLWidget::pass1()
{
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &mesh_shader_pass_idx_1);
	glClear(GL_COLOR_BUFFER_BIT);
	glDepthMask(GL_FALSE);
	drawScene();
	GL_Error();
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

/*void GLWidget::initSelectionFrameBuffer(int width, int height)
{
	// create FBO
	glGenFramebuffers(1, &m_selectionFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_selectionFrameBuffer);

	glGenRenderbuffers(1, &m_selectionRenderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, m_selectionRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_selectionRenderBuffer);

	GLuint depthBuffer;
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}*/

/*int GLWidget::processSelection(float x, float y)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_selectionFrameBuffer);
	glEnable(GL_DEPTH_TEST);

	GLubyte pixel[3];

	glReadPixels(x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, (void*)pixel);
	int pickedID = pixel[0] + pixel[1] * 256 + pixel[2] * 65536;
	qDebug() << pixel[0] << " " << pixel[1] << " " << pixel[2];

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GL_Error();

	return pickedID;
}*/

void GLWidget::load3DTexturesFromRaw(QString path, GLuint& texture, GLenum texture_unit, int sizeX, int sizeY, int sizeZ)
{
	unsigned int size = sizeX * sizeY * sizeZ;
	unsigned char* rawData = (unsigned char*)m_data_container->loadRawFile(path, size);

	if (rawData)
	{
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

		glTexImage3D(GL_TEXTURE_3D, 0, GL_R8, sizeX, sizeY, sizeZ, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, rawData);
	}

	delete[] rawData;
}