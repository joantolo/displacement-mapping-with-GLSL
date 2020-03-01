#version 400 core
 
layout(triangles, equal_spacing, ccw) in;

in vec3 tcColor[];
in vec3 tcPos[];
in vec3 tcNorm[];
in vec2 tcTexCoord[];

out vec3 teColor;
out vec3 tePos;
out vec3 teNorm;
out vec2 teTexCoord;

uniform sampler2D colorTex;
uniform bool applyDisplacement;

vec4 interpolate(in vec4 v0, in vec4 v1, in vec4 v2);
vec3 interpolate(in vec3 v0, in vec3 v1, in vec3 v2);
vec2 interpolate(in vec2 v0, in vec2 v1, in vec2 v2);

void main()
{ 
	teColor = interpolate(
	tcColor[0], 
	tcColor[1],  
	tcColor[2]);

	tePos = interpolate(
	tcPos[0], 
	tcPos[1],  
	tcPos[2]);

	teNorm = interpolate(
	tcNorm[0], 
	tcNorm[1],  
	tcNorm[2]);

	teTexCoord = interpolate(
	tcTexCoord[0], 
	tcTexCoord[1],  
	tcTexCoord[2]);

	vec4 pos = interpolate(
	gl_in[0].gl_Position, 
	gl_in[1].gl_Position, 
	gl_in[2].gl_Position);

	//Desplazamiento por textura de color
	if(applyDisplacement)
	{
		vec4 translation  = length(texture(colorTex, teTexCoord)) * normalize(vec4(tePos, 0));
		tePos = translation.xyz + tePos;
		gl_Position = translation + pos;
	}else
	{
		//sin desplazamiento
		gl_Position = pos;
	}
}

vec4 interpolate(in vec4 v0, in vec4 v1, in vec4 v2)
{
	vec4 l1 = v1 - v0;
	vec4 l0 = v2 - v0;

	return v0 + gl_TessCoord.x * l1 + gl_TessCoord.y * l0;
}


vec3 interpolate(in vec3 v0, in vec3 v1, in vec3 v2)
{
	vec3 l1 = v1 - v0;
	vec3 l0 = v2 - v0;

	return v0 + gl_TessCoord.x * l1 + gl_TessCoord.y * l0;
}


vec2 interpolate(in vec2 v0, in vec2 v1, in vec2 v2)
{
	vec2 l1 = v1 - v0;
	vec2 l0 = v2 - v0;

	return v0 + gl_TessCoord.x * l1 + gl_TessCoord.y * l0;
}