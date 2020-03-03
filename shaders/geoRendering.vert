#version 330 core
in vec3 inPos;	

out vec3 color;

void main()
{
	color = vec3(1,1,0);
	gl_Position = vec4(inPos, 1.0);
}