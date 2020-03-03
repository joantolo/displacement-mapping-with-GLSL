#version 330 core
layout(triangles) in;
layout(line_strip, max_vertices = 8) out;

in vec3 color[];

out vec3 fcolor;

uniform mat4 modelViewProj;

const float MAGNITUDE = 1.8;

// Thanks to https://learnopengl.com/Advanced-OpenGL/Geometry-Shader

void PassInfo(int index)
{
    fcolor = color[index];
}

void GenerateNormal()
{
    vec3 L0 = (gl_in[0].gl_Position - gl_in[1].gl_Position).xyz;
    vec3 L1 = (gl_in[2].gl_Position - gl_in[1].gl_Position).xyz;
    vec4 normal = vec4(normalize(cross(L1, L0)).xyz,0);

    // Center of the triangle
    vec4 normalPoint = ((gl_in[0].gl_Position + gl_in[1].gl_Position + gl_in[2].gl_Position) / 3.0);
                                
    //Base of the line
    gl_Position = modelViewProj * normalPoint;
    EmitVertex();

    //Height of the line
    gl_Position = modelViewProj * (normalPoint + normal * MAGNITUDE);
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