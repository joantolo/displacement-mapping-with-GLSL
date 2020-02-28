#version 330 core
layout(triangles) in;
layout(line_strip, max_vertices = 6) out;

in vec4 norm[];
in vec3 color[];
in vec2 textCoord[];

out vec4 fnorm;
out vec3 fcolor;
out vec2 ftextCoord;

const float MAGNITUDE = 1.2;

void GenerateLine(int index)
{
    fnorm = norm[index];
    fcolor = color[index];
    ftextCoord = textCoord[index];

    gl_Position = gl_in[index].gl_Position;
    EmitVertex();

    gl_Position = gl_in[index].gl_Position + vec4(norm[index].xyz,0.0)* MAGNITUDE;
    EmitVertex();

    EndPrimitive();
}

void main()
{
    GenerateLine(0); // first vertex normal
    GenerateLine(1); // second vertex normal
    GenerateLine(2); // third vertex normal


}  