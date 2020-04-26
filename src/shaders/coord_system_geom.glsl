#version 430

in vec3 geom_color[];
out vec3 frag_color;

layout(lines) in;
layout(line_strip, max_vertices = 2) out;

void main() {
  for(int i = 0; i < 2; i++) 
  {
    gl_Position = gl_in[i].gl_Position;
    frag_color = geom_color[i];
    EmitVertex();
  }
  EndPrimitive();
}

