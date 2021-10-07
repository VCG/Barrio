#pragma once

#include <QVector3D>
#include <QMatrix4x4>
#include <QPoint>
#include <QMouseEvent>

#include <qmath.h>
#include <math.h>

#include "globalParameters.h"


class Camera
{
	public:
		Camera(float fov, float near, float far, QVector3D center);
		~Camera();

		void frameUpdate();
		void setAspectRatio(qreal aspect_ratio);

		QVector3D getPosition();

	    QMatrix4x4& getModelMatrix();
		QMatrix4x4& getViewMatrix();
		QMatrix4x4& getProjectionMatrix();

		void resetCamera();
		void mouse_press_event(QMouseEvent* event);
		void mouse_move_event(QMouseEvent* event);
		void processScroll(double delta);

		static int normalize_angle(int angle);

		void setXRotation(int angle);
		void setYRotation(int angle);
		void setZRotation(int angle);

	private:

		QMatrix4x4 model;
		QMatrix4x4 view;
		QMatrix4x4 projection;

		QVector3D center;
		QVector3D position;
		QVector3D front;
		QVector3D up;

		QPoint last_mouse_position;
		
		int x_rotation;
		int y_rotation;
		int z_rotation;

		float fov;
		float speed;
		float distance;
		float nearPlane;
        float farPlane;
};

