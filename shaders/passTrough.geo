#version 330 core 
layout ( triangles ​) in; 
layout ( triangles, max_vertices=1 ​) out;

void main()
{
	gl_Position= gl_in[0].gl_Position;
	EmitVertex();
	EndPrimitive();
}