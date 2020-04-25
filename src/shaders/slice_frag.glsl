#version 430
out vec4 FragColor;

in vec2 texCoord;
in float xy_slice_z;

uniform sampler3D volume;

void main()
{
    float depth_normed = xy_slice_z / 5.0;
    float tex_value = texture(volume, vec3(texCoord.x, texCoord.y, depth_normed)).x;
    FragColor = vec4(tex_value, tex_value, tex_value, 1.0);
}