#version 330 core
layout(triangles) in;
layout(points, max_vertices = 6) out;

in vec4 norm[];
in vec3 color[];
in vec2 textCoord[];

out vec4 fnorm;
out vec3 fcolor;
out vec2 ftextCoord;

const float MAGNITUDE = 1.2;

// Thanks to https://learnopengl.com/Advanced-OpenGL/Geometry-Shader

void PointInPlace(int index)
{
    fnorm = norm[index];
    fcolor = color[index];
    ftextCoord = textCoord[index];

    gl_Position = gl_in[index].gl_Position;
    EmitVertex();    

    gl_Position = gl_in[index].gl_Position ;
    EmitVertex();   

    gl_Position = gl_in[index].gl_Position ;
    EmitVertex();   

    gl_Position = gl_in[index].gl_Position ;
    EmitVertex();

    EndPrimitive();
}

void main()
{
    PointInPlace(0); // first vertex normal
    PointInPlace(1); // second vertex normal
    PointInPlace(2); // third vertex normal
}  