#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QWidget>
#include <QtOpenGL>

#include "mainopengl.h"

#include "abstractionspace.h"
#include "inputform.h"
#include "globalParameters.h"
#include "camera.h"


enum BufferNames { COUNTER_BUFFER = 0, LINKED_LIST_BUFFER };

struct ListNode {
	QVector4D color;
	GLfloat depth;
	GLuint next;
};

struct SharedGLResources
{
	QOpenGLBuffer*			vertex_vbo;
	QOpenGLBuffer*			mesh_index_vbo;
	QOpenGLBuffer*			skeleton_index_vbo;

	int						mesh_index_count;
	int						skeleton_index_count;

	GLuint*					mito_cell_distance_colormap;
	GLuint*					image_stack_volume;

	int						currently_hovered_widget;

	QOpenGLBuffer*			slice_vertex_vbo;

	bool					show_3d_renderings;
	bool					show_color_coding;
	bool					show_slice;
	bool					show_related_synapses;
	bool					show_silhouette;

	float					cell_opacity;
	float					slice_depth;
	double					distance_threshold;

	QVector<int>*			highlighted_objects;
	QVector<int>*			highlighted_group_boxes;
	QVector<int>*			widget_queue;

	QVector<int>*			all_selected_mitos;
};

class GLWidget : public QOpenGLWidget, MainOpenGL
{
	Q_OBJECT

public:
	GLWidget(int hvgx_id, SharedGLResources* resources, bool isOverviewWidget, QWidget* parent = 0);
	~GLWidget();
	void init(DataContainer* data_container, bool use_camera_settings, CameraSettings cameraSettings = CameraSettings());

	DataContainer* getDataContainer() { return m_data_container; }

	//int pickObject(QMouseEvent* event);

	void drawScene();
	void updateHighlightedSSBO();
	void updateVisibilitySSBO();
	void setVisibleStructuresSingelObject();
	void setVisibleStructuresOverView();
	void setVisibleStructures(int id);
	CameraSettings getCameraSettings();
	void update_synapse_distance_threshold(double distance);
	void update_visibility();
	void updateVisParameters();
	void resetCamera();
	void zoom(double delta);

	void pass1();
	void pass2();
	void clearBuffers();

	void initMeshShaderStorage(int width, int height);
	//void initSelectionFrameBuffer(int width, int height);
	//int processSelection(float x, float y);

	void load3DTexturesFromRaw(QString path, GLuint& texture, GLenum texture_unit, int sizeX, int sizeY, int sizeZ);

public slots:
	void prepareResize();
	void ShowContextMenu(const QPoint&);

protected:
	void initializeGL() Q_DECL_OVERRIDE;
	void resizeGL(int w, int h) Q_DECL_OVERRIDE;
	void paintGL() Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void wheelEvent(QWheelEvent* event) Q_DECL_OVERRIDE;
	void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;
	void enterEvent(QEvent* event) Q_DECL_OVERRIDE;
	void leaveEvent(QEvent* event) Q_DECL_OVERRIDE;

	void updateMVPAttrib(QOpenGLShaderProgram* program);

	DataContainer*						m_data_container;
	Camera*								camera;

	SharedGLResources*					m_shared_resources;

	int                                 m_selected_hvgx_id;
	int                                 m_parent_id;

	QOpenGLShaderProgram*				m_mesh_program;
	QOpenGLVertexArrayObject            m_mesh_vao;

	QOpenGLShaderProgram*				m_slice_program;
	QOpenGLVertexArrayObject            m_slice_vao;

	std::vector<int>                    m_visible_structures; // list of hvgx ids with visible structures
	GLuint                              m_visibility_ssbo;
	GLuint                              m_highlighted_ssbo;

	GLuint                              m_selectionFrameBuffer;
	GLuint                              m_selectionRenderBuffer;

	/* order independent transparency */
	QOpenGLVertexArrayObject            m_fsQuad_vao;
	GLuint                              oit_buffers[2], headPtrTex;
	GLuint                              mesh_shader_pass_idx_1, mesh_shader_pass_idx_2;
	GLuint                              clear_oit_buffers;
	GLint                               m_maxNodes;

	std::vector<GLuint>*				headPtrClearBuf;

	int                                 m_width, m_height;
	bool                                m_init;
	bool                                m_is_overview_widget;
	QVector<int>*						m_all_selected_mitos;
	double                              m_distance_threshold;
	QWidget*							m_parent;
	GLuint								m_image_stack_texture;
};
#endif // GLWIDGET_H
