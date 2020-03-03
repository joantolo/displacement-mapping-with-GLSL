#version 330 core
layout(triangles) in;
layout(points, max_vertices = 6) out;

in vec3 color[];


uniform mat4 modelViewProj;

out vec3 fcolor;

// Thanks to https://learnopengl.com/Advanced-OpenGL/Geometry-Shader

void PointInPlace(int index)
{
    fcolor = color[index];
    gl_Position = modelViewProj * gl_in[index].gl_Position;
    EmitVertex();    

    EndPrimitive();
}

void main()
{
    PointInPlace(0); // first vertex normal
    PointInPlace(1); // second vertex normal
    PointInPlace(2); // third vertex normal
}  