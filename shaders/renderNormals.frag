#version 330 core

layout(location = 0) out vec4 outColor;

in vec3 fcolor;
in vec4 fnorm;
in vec2 ftexCoord;

uniform mat4 normal;
uniform mat4 modelView;
uniform mat4 modelViewProj;


void main()
{
  outColor =  vec4(fcolor,1.0);
}  