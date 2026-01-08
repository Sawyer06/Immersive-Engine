#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 proj;
uniform mat4 view;

void main()
{
    TexCoords = aPos;
    mat4 view3 = mat4(mat3(view)); // Take out translation.
    vec4 pos = proj * view3 * vec4(aPos, 1.0);
    gl_Position = pos.xyww; // Forces z to always equal 1.
}