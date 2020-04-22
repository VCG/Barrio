#version 430

in vec2 vertUV[];
out vec2 TexCoord;

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

void main() {
  for(int i = 0; i < 3; i++) { // You used triangles, so it's always 3
    gl_Position = gl_in[i].gl_Position;
    TexCoord = vertUV[i];
    EmitVertex();
  }
  EndPrimitive();
}

