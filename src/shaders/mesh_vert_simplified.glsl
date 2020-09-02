#version 430 core

#define SLICE 10

layout (location = 0) in vec4 mesh_vtx;
layout (location = 1) in float distance_to_cell;
layout (location = 2) in int structure_type;
layout (location = 3) in int hvgxID;
layout (location = 4) in vec4 normal;

uniform mat4    mMatrix;
uniform mat4    vMatrix;
uniform mat4    pMatrix;
uniform mat4    rMatrix;

out vec4 normal_frag;
out vec3 eye_frag;
flat out int frag_structure_type;
out float frag_cell_distance;
out vec4 frag_vert_pos;
flat out int frag_hvgx;

vec3 eye = vec3(0.5, 0.5, 1.0);


	

void main()
{
    mat4 mvpMatrix = pMatrix * vMatrix * mMatrix;
    if(structure_type == SLICE)
    {
      gl_Position =  mvpMatrix * vec4(mesh_vtx.yz, 2.0, 1.0);
    }
    else
    {
      gl_Position =  mvpMatrix * vec4(mesh_vtx.xyz, 1.0);
    }

    

    normal_frag = normalize(normal);
    eye_frag = normalize(eye);
    frag_structure_type = structure_type;
    frag_cell_distance = distance_to_cell;
    frag_vert_pos = /*rMatrix */ mesh_vtx;
    frag_hvgx = hvgxID;
}