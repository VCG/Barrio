#pragma once

#include <QVector3D>
#include <QMatrix4x4>

#include "globalParameters.h"

class Camera
{
	public:
		Camera(float fov, float aspectRatio, float near, float far);
		~Camera();

		void frameUpdate(float deltaT, float time);
		void setPosition(float x, float y, float z);
		void setPosition(const QVector3D& pos);
		void setLookAt(const QVector3D& lookAt);
		void setFront(const QVector3D& newFront);
		void switchMovement();

		const QVector3D& getPosition() const;
		const QVector3D& getFront() const;
		const QVector3D& getUp() const;
		const QVector3D& getViewMatrix() const;
		const QVector3D& getProjection() const;
		const QVector3D& getVP() const;
		const float& getYaw() const;
		const float& getPitch() const;
		const QVector3D getLightDirection();

		void processKeyInput(float frameDeltaTime);
		void processScroll(double yoffset);
		void resetCamera();

		// Frustum Plane Distances
		float nearPlane = 0.1f;
		float farPlane = 40.f;

		float speed = 100.00f;

	private:

		void initPaths();

		QMatrix4x4 viewMatrix;
		QMatrix4x4 projection;

		QMatrix4x4 position;
		QMatrix4x4 front;
		QMatrix4x4 up;

		int currentPoint;
		QVector3D target;

		float pitch;
		float yaw;
		float distance;
		bool moveFree;

		void calculateFront();

};

