#version 330 core
layout(triangles) in;
layout(line_strip, max_vertices = 6) out;

in vec3 color[];

out vec3 fcolor;

uniform mat4 modelViewProj;

const float MAGNITUDE = 1.2;

// Thanks to https://learnopengl.com/Advanced-OpenGL/Geometry-Shader

void GenerateLine(int index)
{
	vec3 L0 = (gl_in[0].gl_Position - gl_in[1].gl_Position).xyz;
    vec3 L1 = (gl_in[2].gl_Position - gl_in[1].gl_Position).xyz;
    vec4 normal = vec4(normalize(cross(L1, L0)).xyz, 0);

    fcolor = color[index];

    gl_Position = modelViewProj * gl_in[index].gl_Position;
    EmitVertex();

    gl_Position = modelViewProj * (gl_in[index].gl_Position + normal * MAGNITUDE);
    EmitVertex();

    EndPrimitive();
}

void main()
{
    GenerateLine(0); // first vertex normal
    GenerateLine(1); // second vertex normal
    GenerateLine(2); // third vertex normal
}  