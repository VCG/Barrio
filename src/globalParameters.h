#pragma once

#define MESH_MAX_X    5.0f
#define MESH_MAX_Y    5.0f
#define MESH_MAX_Z    5.0f

#define DIM_X       999
#define DIM_Y       999
#define DIM_Z       449
#define DIM_G		64

#define CONSIDER_CLOSE 1.0

#define PATH        QString("C:/Users/jakobtroidl/Desktop/NeuroKit/")

#define RECOMPUTE_DATA FALSE

#include <QList>
#include <QJsonObject>

enum NumberOfEntities
{
	LOW, MEDIUM, HIGH
};

struct GlobalVisParameters
{
	bool needs_update = false;
	double distance_threshold;
	float opacity;
	float slice_depth;

	QList<int> selected_objects;
	QVector<int> highlighted_objects;
	QVector<int> highlighted_group_boxes;
	QVector<int> my_add_queue;
};

struct SpecificVisParameters
{
	int number_of_bins = 20;
	QString colors;
	QJsonObject settings;
};
