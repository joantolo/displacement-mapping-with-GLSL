#version 330 core

in vec3 inPos;
in vec3 inNormal;

uniform mat4 normal;
uniform mat4 modelView;
uniform mat4 modelViewProj;

out vec3 pos;
out vec3 norm;

out VS_OUT {
    vec3 normal;
} vs_out;



void main()
{
	norm = (normal * vec4(inNormal, 0.0)).xyz;
	pos = (modelView * vec4(inPos, 1.0)).xyz;

	gl_Position= modelViewProj * vec4(inPos, 1.0);
}