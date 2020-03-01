#version 330 core
layout(triangles) in;
layout(line_strip, max_vertices = 8) out;

in vec4 norm[];
in vec3 color[];
in vec2 textCoord[];

out vec4 fnorm;
out vec3 fcolor;
out vec2 ftextCoord;

uniform mat4 modelViewProj;

const float MAGNITUDE = 1.8;

// Thanks to https://learnopengl.com/Advanced-OpenGL/Geometry-Shader

void PassInfo(int index)
{
    fnorm = norm[index];
    fcolor = color[index];
    ftextCoord = textCoord[index];
}

void GenerateNormal()
{
    vec3 L0= (gl_in[0].gl_Position - gl_in[1].gl_Position).xyz;
    vec3 L1= (gl_in[2].gl_Position - gl_in[1].gl_Position).xyz;
    vec4 normal= vec4(normalize(cross(L0,L1)).xyz,0);

    // Center of the triangle
    vec4 normalPoint = ((gl_in[0].gl_Position + gl_in[1].gl_Position + gl_in[2].gl_Position) / 3.0);
                                
    //Base of the line
    gl_Position = normalPoint;
    EmitVertex();

    //Height of the line
    gl_Position =  (normalPoint + normal * MAGNITUDE);
    EmitVertex();

    EndPrimitive();
}

void main()
{
    PassInfo(0); 
    PassInfo(1); 
    PassInfo(2); 

    GenerateNormal();

}  