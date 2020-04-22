#version 430
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler3D volume;

void main()
{
    FragColor = texture(volume, vec3(TexCoord, 0.5));
}