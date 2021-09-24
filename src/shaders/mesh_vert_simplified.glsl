#version 430 core

#define SLICE 10

layout (location = 0) in vec4     mesh_vtx;
layout (location = 1) in float    distance_to_cell;
layout (location = 2) in int      structure_type;
layout (location = 3) in int      hvgxID;
layout (location = 4) in vec4     normal;
layout (location = 5) in int      is_skeleton;

uniform mat4    mMatrix;
uniform mat4    vMatrix;
uniform mat4    pMatrix;
uniform vec3    eye;

uniform float slice_z;
uniform int   is_overview;
uniform int   currently_hovered_id;

out vec4          frag_normal;
out vec4          frag_camera_position;
flat out int      frag_structure_type;
out float         frag_cell_distance;
out vec4          frag_vert_pos;
flat out int      frag_hvgx;
flat out int      frag_is_skeleton;
out float         frag_slice_z;

flat out int         frag_is_overview;
flat out int         frag_currently_hovered_id;

void main()
{
    mat4 vpMatrix = pMatrix * vMatrix;
    if(structure_type == SLICE)
    {
      gl_Position =  vpMatrix * vec4(mesh_vtx.y, slice_z, mesh_vtx.z, 1.0);
    }
    else
    {
      gl_Position =  vpMatrix * vec4(mesh_vtx.xyz, 1.0);
    }

    frag_normal = normal;
    frag_camera_position = vec4(eye, 1.0);
    frag_vert_pos = mesh_vtx;

    frag_structure_type = structure_type;
    frag_cell_distance = distance_to_cell;
    frag_hvgx = hvgxID;
    frag_slice_z = slice_z;
    frag_is_skeleton = is_skeleton;

    frag_currently_hovered_id = currently_hovered_id;
    frag_is_overview = is_overview;
}