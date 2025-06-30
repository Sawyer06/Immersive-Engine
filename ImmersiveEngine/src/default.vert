#version 330 core
layout(location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColor;
layout(location = 3) in vec2 aTex;

out vec3 color;

out vec2 texCoord;

out vec3 normal;
out vec3 worldPos;

uniform float scale;
uniform mat4 transform;
uniform mat4 view;
uniform mat4 proj;

void main()
{
    worldPos = vec3(transform * vec4(aPos, 1.0f)); // Calculate the world space of the vertex.
    normal = mat3(transpose(inverse(transform))) * aNormal; // Preserves the normal directions of the normals through object scaling (inverse) and rotation (transpose).

    gl_Position = proj * view * vec4(worldPos, 1.0f);
    //gl_Position = transform * vec4(aPos, 1.0) * scale;
    //gl_Position = vec4(aPos.x + aPos.x * scale, aPos.y + aPos.y * scale, aPos.z + aPos.z * scale, 1.0);
    color = aColor;
    texCoord = aTex;
}