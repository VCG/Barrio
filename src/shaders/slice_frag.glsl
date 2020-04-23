#version 430
out vec4 FragColor;

in vec2 texCoord;

uniform sampler3D volume;

void main()
{
    FragColor = texture(volume, vec3(texCoord, 0.6));
}