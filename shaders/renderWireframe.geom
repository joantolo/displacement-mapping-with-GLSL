#version 330 core
layout(triangles) in;
layout(line_strip, max_vertices = 6) out;

in vec3 color[];

out vec3 fcolor;

uniform mat4 modelViewProj;

// Thanks to https://learnopengl.com/Advanced-OpenGL/Geometry-Shader

void GenerateLine(int index)
{
    fcolor = color[index];
    gl_Position = modelViewProj * gl_in[index].gl_Position;
    EmitVertex();
}

void main()
{

    GenerateLine(0); // first vertex normal
    GenerateLine(1); // second vertex normal
    GenerateLine(2); // third vertex normal
    GenerateLine(0); // first vertex normal

    EndPrimitive();

}  