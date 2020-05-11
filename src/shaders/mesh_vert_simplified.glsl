#version 430 core
layout (location = 0) in vec4 mesh_vtx;
layout (location = 1) in float distance_to_cell;
layout (location = 2) in int structure_type;
layout (location = 3) in int hvgxID;

//layout (location = 4) in vec4 normal;

uniform mat4    mMatrix;
uniform mat4    vMatrix;
uniform mat4    pMatrix;
uniform mat4    rMatrix;

out float hvgx_frag;
out vec4 normal_frag;
out vec3 eye_frag;
out flat int frag_structure_type;

vec3 eye = vec3(0.5, 0.5, 1.0);

void main()
{
    mat4 pvmMatrix = pMatrix * vMatrix * mMatrix;
    vec4 out_vtx =  pvmMatrix * vec4(mesh_vtx.xyz, 1.0);
    gl_Position = vec4(out_vtx.xyz, 1.0);

    normal_frag = vec4(1.0, 0.0, 0.0, 1.0);//normalize(normal);
    eye_frag = normalize(eye);
    frag_structure_type = structure_type;
    hvgx_frag = float(hvgxID);
}