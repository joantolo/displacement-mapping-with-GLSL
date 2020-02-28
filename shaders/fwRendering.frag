#version 400 core

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outPosVertex;
layout(location = 2) out vec4 outNormalVertex;
layout(location = 3) out vec4 outEmiTex;

in vec3 teColor;
in vec3 tePos;
in vec3 teNorm;
in vec2 teTexCoord;

uniform sampler2D colorTex;
uniform sampler2D emiTex;


void main()
{
	outPosVertex = vec4(tePos.xyz, 1);

	outNormalVertex = vec4(normalize(teNorm), 0);

	outEmiTex = vec4(0); //texture(emiTex, teTexCoord);

	outColor = texture(colorTex, teTexCoord);
}
