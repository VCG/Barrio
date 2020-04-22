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

out vec2 TexCoord;
//out vec4 V_worldPos;

void main()
{

    mat4 pvmMatrix = pMatrix * vMatrix * mMatrix;
    //V_worldPos = rMatrix * vec4(pos, 1.0);

    gl_Position = pvmMatrix * vec4(pos, 1.0);
    TexCoord = texCoord;
}