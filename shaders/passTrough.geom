#version 330 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 1) out;

in vec3 color[];

out vec3 fcolor;

void main()
{
	gl_Position= gl_in[0].gl_Position ;
	fcolor= color[0];
	EmitVertex();
	EndPrimitive();
}