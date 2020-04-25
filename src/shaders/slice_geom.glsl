#version 430

in vec2 TexCoord[];
in float out_slice_z[];

out vec2 texCoord;
out float xy_slice_z;

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

void main() {
  for(int i = 0; i < 3; i++) { // You used triangles, so it's always 3
    gl_Position = gl_in[i].gl_Position;
    texCoord = TexCoord[i];
    xy_slice_z = out_slice_z[i];
    EmitVertex();
  }
  EndPrimitive();
}

