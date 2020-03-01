#version 400 core
 
layout(vertices = 3) out;

in vec3 vColor[];
in vec3 vPos[];
in vec3 vNorm[];
in vec2 vTexCoord[];

out vec3 tcColor[];
out vec3 tcPos[];
out vec3 tcNorm[];
out vec2 tcTexCoord[];

uniform int outerLevel;
uniform int innerLevel;
 
void main(void)
{
	 gl_TessLevelOuter[0] = outerLevel;
	 gl_TessLevelOuter[1] = outerLevel;
	 gl_TessLevelOuter[2] = outerLevel;
 
	 gl_TessLevelInner[0] = innerLevel;
	 
	tcColor[gl_InvocationID] = vColor[gl_InvocationID];

	tcPos[gl_InvocationID] = vPos[gl_InvocationID];

	tcNorm[gl_InvocationID] = vNorm[gl_InvocationID];

	tcTexCoord[gl_InvocationID] = vTexCoord[gl_InvocationID];
 
	 gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
