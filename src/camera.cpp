#include "camera.h"

Camera::Camera(float fov, float aspectRatio, float near, float far)
{
	this->position = QVector3D(MESH_MAX_X / 2.0, MESH_MAX_Y / 2.0, 2.0 * MESH_MAX_Z);
}

Camera::~Camera()
{
}
