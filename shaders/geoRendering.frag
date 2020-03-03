#version 400 core

layout(location = 0) out vec4 outColor;

in vec3 fcolor;

void main()
{
  outColor =  vec4(fcolor, 1.0);
}  