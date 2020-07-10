#pragma once

#define MESH_MAX_X    5.0f
#define MESH_MAX_Y    5.0f
#define MESH_MAX_Z    5.0f

#define DIM_X       999
#define DIM_Y       999
#define DIM_Z       449
#define DIM_G		64

#define RECOMPUTE_DATA FALSE

struct GlobalVisParameters
{
  double distance_threshold;
  float opacity;
  float slice_depth;
};
