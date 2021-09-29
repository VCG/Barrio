#include "camera.h"

Camera::Camera(float fov, float near, float far, QVector3D center)
{
	this->position = QVector3D(MESH_MAX_X / 2.0, MESH_MAX_Y / 2.0, 2.0 * MESH_MAX_Z);
	this->front = QVector3D(0.0f, 0.0f, 1.0f);
	this->up = QVector3D(0.0f, 1.0f, 0.0);
	this->center = center;

	this->distance = 50.0f;
	this->fov = fov;
	this->pitch = 0.0f;
	this->yaw = 0.0f;
	this->speed = 0.5;

	this->nearPlane = near;
	this->farPlane = far;
}

Camera::~Camera()
{
}

void Camera::frameUpdate()
{
	calculateFront();
	this->position = -this->front * this->distance;
	this->viewMatrix.setToIdentity();
	this->viewMatrix.lookAt(this->position, center, this->up);
}

void Camera::setAspectRatio(qreal aspect_ratio)
{
	float fov_rad = qDegreesToRadians(this->fov);
	this->projection.setToIdentity();
	this->projection.perspective(fov_rad, aspect_ratio, this->nearPlane, this->farPlane);
}

const QVector3D& Camera::getPosition() const
{
	return this->position;
}

const QVector3D& Camera::getFront() const
{
	return this->front;
}

const QVector3D& Camera::getUp() const
{
	return this->up;
}

void Camera::calculateFront()
{
	float yaw_rad = qDegreesToRadians(this->yaw);
	float pitch_rad = qDegreesToRadians(this->pitch);
	front.setX(cos(pitch_rad) * cos(yaw_rad));
	front.setY(sin(pitch_rad));
	front.setZ(cos(pitch_rad) * sin(yaw_rad));
	front.normalize();
}

void Camera::processMouseInput(float deltaX, float deltaY)
{
	this->yaw += deltaX * this->speed;
	this->pitch += deltaY * this->speed;

	if (this->pitch > 89.0f)
		this->pitch = 89.0f;
	if (this->pitch < -89.0f)
		this->pitch = -89.0f;
}

void Camera::processScroll(int delta)
{
	if (delta < 0) {
	  this->distance *= 0.9;
	}
	else {
		this->distance *= 1.1;
	}
}

void Camera::resetCamera()
{
}

const QMatrix4x4& Camera::getViewMatrix() const
{
	return this->viewMatrix;
}

const QMatrix4x4& Camera::getProjection() const
{
	return this->projection;
}

const QMatrix4x4& Camera::getVP() const
{
	return this->projection * this->viewMatrix;
}

const float& Camera::getYaw() const
{
	return this->yaw;
}

const float& Camera::getPitch() const
{
	return this->pitch;
}