#version 430
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texCoord;

// World transformation
uniform mat4    mMatrix;

// View Transformation
uniform mat4    vMatrix;

// Projection transformation
uniform mat4    pMatrix;
uniform mat4    rMatrix;

uniform float slice_z;

out vec2 TexCoord;
out float out_slice_z;
//out vec4 V_worldPos;

void main()
{
    mat4 pvmMatrix = pMatrix * vMatrix * mMatrix;
    vec3 out_pos = vec3(pos.yz, -slice_z);
    gl_Position = pvmMatrix * vec4(out_pos, 1.0);
    
    TexCoord = texCoord;
    out_slice_z = slice_z;
}