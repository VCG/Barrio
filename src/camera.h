#pragma once

#include <QVector3D>
#include <QMatrix4x4>
#include <qmath.h>
#include <math.h>

#include "globalParameters.h"

class Camera
{
	public:
		Camera(float fov, float near, float far, QVector3D center);
		~Camera();

		void frameUpdate();
		void setPosition(float x, float y, float z);
		void setPosition(const QVector3D& pos);
		void setLookAt(const QVector3D& lookAt);
		void setFront(const QVector3D& newFront);
		void switchMovement();
		void setAspectRatio(qreal aspect_ratio);

		const QVector3D& getPosition() const;
		const QVector3D& getFront() const;
		const QVector3D& getUp() const;
		const QMatrix4x4& getViewMatrix() const;
		const QMatrix4x4& getProjection() const;
		const QMatrix4x4& getVP() const;
		const float& getYaw() const;
		const float& getPitch() const;

		void processMouseInput(float deltaX, float deltaY);
		void processScroll(int delta);
		void resetCamera();

	private:

		QMatrix4x4 viewMatrix;
		QMatrix4x4 projection;

		QVector3D center;
		QVector3D position;
		QVector3D front;
		QVector3D up;

		float fov;

		float speed;

		float yaw;
		float pitch;
		float distance;
		float nearPlane;
        float farPlane;

		void calculateFront();

};

