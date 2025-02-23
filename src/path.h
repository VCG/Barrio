#ifndef PATH_H
#define PATH_H

#include "mainopengl.h"

class Path : public MainOpenGL
{
public:
	Path(int ID);
	Path(Path*);
	~Path();

	void updatePath(Path*);

	void initPath();
	void drawPath(QMatrix4x4 projection);

	void tracePath(QMatrix4x4 projection, int x);

	void addPoint(QVector2D point, QVector2D selection);
	void resetPath() { m_path.clear();  m_selectionPath.clear(); }
	void updateRecordingFlag(bool flag) { m_recording = flag; }
	QVector2D getXY(int x);

	void namePath(QString name) { m_name = name; }
	void setID(int ID) { m_ID = ID; }
	void addNote(QString note) { m_note = note; }

	QString getName() { return m_name; }

	void dumpPath();
	void loadPath();

	bool getRecordingFlag() { return m_recording; }
	int getID() { return m_ID; }
	QString getNote() { return m_note; }
	std::vector<QVector2D> getPath() { return m_path; }
	std::vector<QVector2D> getSelectionPath() { return m_selectionPath; }


protected:
	QString                     m_name;
	int                         m_ID;
	QString                     m_note;

	std::vector<QVector2D>      m_path;
	std::vector<QVector2D>      m_selectionPath;

	QOpenGLVertexArrayObject    m_vao;
	QOpenGLBuffer               m_vbo;
	GLuint                      m_program;

	bool                        m_recording;

	std::vector<int>            m_objects_list; // list of visible objects ?
};

#endif // PATH_H
