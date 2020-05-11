#version 430

#define MITO  1
#define AXONS 2
#define BOUTN 3
#define DENDS 4
#define SPINE 5
#define ASTRO 6
#define SYNPS 7

layout (location = 0) out vec4 FragColor;

in vec3         normal_frag;
in vec3			eye_frag;
flat in int     frag_structure_type;
in float        hvgx_frag;

void main()
{
  FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}