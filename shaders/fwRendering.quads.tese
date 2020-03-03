#version 400 core
 
layout(quads, equal_spacing, ccw) in;

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

uniform mat4 modelViewProj;
uniform mat4 modelView;

//Interpolation method from http://voxels.blogspot.com/2011/09/tesselation-shader-tutorial-with-source.html
vec4 interpolate(in vec4 v0, in vec4 v1, in vec4 v2, in vec4 v3);
vec3 interpolate(in vec3 v0, in vec3 v1, in vec3 v2, in vec3 v3);
vec2 interpolate(in vec2 v0, in vec2 v1, in vec2 v2, in vec2 v3);

const float MAGNITUDE = 1.8;

void main()
{ 
	teColor = interpolate(
	tcColor[0], 
	tcColor[1],  
	tcColor[2],  
	tcColor[3]);

	tePos = interpolate(
	tcPos[0], 
	tcPos[1],  
	tcPos[2],  
	tcPos[3]);

	teNorm = interpolate(
	tcNorm[0], 
	tcNorm[1],  
	tcNorm[2],  
	tcNorm[3]);

	teTexCoord = interpolate(
	tcTexCoord[0], 
	tcTexCoord[1],  
	tcTexCoord[2],  
	tcTexCoord[3]);

	vec4 pos = interpolate(
	gl_in[0].gl_Position, 
	gl_in[1].gl_Position, 
	gl_in[2].gl_Position, 
	gl_in[3].gl_Position);

	//Desplazamiento por textura de color
	if(applyDisplacement)
	{
		vec4 translation = MAGNITUDE * length(texture(colorTex, teTexCoord)) * normalize(vec4(tePos, 0));
		tePos = (modelView * vec4((translation.xyz + tePos), 1)).xyz;
		gl_Position = modelViewProj * (translation + pos);
	}else
	{
		//sin desplazamiento
		tePos = (modelView * vec4(tePos, 1)).xyz;
		gl_Position = modelViewProj * pos;
	}
}

vec4 interpolate(in vec4 v0, in vec4 v1, in vec4 v2, in vec4 v3)
{
	 vec4 a = mix(v0, v1, gl_TessCoord.x);
	 vec4 b = mix(v3, v2, gl_TessCoord.x);
	 return mix(a, b, gl_TessCoord.y);
}


vec3 interpolate(in vec3 v0, in vec3 v1, in vec3 v2, in vec3 v3)
{
	 vec3 a = mix(v0, v1, gl_TessCoord.x);
	 vec3 b = mix(v3, v2, gl_TessCoord.x);
	 return mix(a, b, gl_TessCoord.y);
}


vec2 interpolate(in vec2 v0, in vec2 v1, in vec2 v2, in vec2 v3)
{
	 vec2 a = mix(v0, v1, gl_TessCoord.x);
	 vec2 b = mix(v3, v2, gl_TessCoord.x);
	 return mix(a, b, gl_TessCoord.y);
}