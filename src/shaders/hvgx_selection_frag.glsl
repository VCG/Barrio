#version 430

in float        hvgx_frag;
out vec4        FragColor;

void main() 
{
    float blue =  int(hvgx_frag / pow(256, 0)) % 256;
    float green = int(hvgx_frag / pow(256, 1)) % 256;
    float red =   int(hvgx_frag / pow(256, 2)) % 256;

    // deal with parent and child overlapping IDs
    FragColor =   vec4(blue/255.0, green/255.0, 0, 1.0);
}
