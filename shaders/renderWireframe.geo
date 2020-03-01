#version 330 core
layout(triangles) in;
layout(line_strip, max_vertices = 6) out;

in vec4 norm[];
in vec3 color[];
in vec2 textCoord[];

out vec4 fnorm;
out vec3 fcolor;
out vec2 ftextCoord;

uniform mat4 normal;
uniform mat4 modelView;
uniform mat4 modelViewProj;

const float MAGNITUDE = 1.2;

// Thanks to https://learnopengl.com/Advanced-OpenGL/Geometry-Shader

void GenerateLine(int index)
{
    fnorm = norm[index];
    fcolor = color[index];
    ftextCoord = textCoord[index];

    gl_Position = modelViewProj* gl_in[index].gl_Position;
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