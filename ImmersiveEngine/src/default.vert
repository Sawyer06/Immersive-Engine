#version 330 core
layout(location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColor;
layout(location = 3) in vec2 aTex;
layout(location = 4) in ivec4 aBoneIDs;
layout(location = 5) in vec4 aBoneWeights;

out vec3 color;

out vec2 texCoord;

out vec3 normal;
out vec3 worldPos;

uniform float scale;
uniform vec2 offset;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 proj;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 boneTransformMats[MAX_BONES];

mat4 calculateAnimatedTransform()
{
    mat4 animatedTransform = mat4(0.0f);
    float totalWeight = 0.0;
    for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
    {
        if (aBoneIDs[i] <= -1 || aBoneIDs[i] >= MAX_BONES)
            continue;

        animatedTransform += boneTransformMats[aBoneIDs[i]] * aBoneWeights[i];
        totalWeight += aBoneWeights[i];
    }
    if (animatedTransform == mat4(0.0))
        color = vec3(0.0);
    else
        color = vec3(1.0);
    return animatedTransform == mat4(0.0) ? mat4(1.0) : animatedTransform;
}

void main()
{
    mat4 transformation = transform * calculateAnimatedTransform();//vec4(aPos, 1.0f);
    //worldPos = vec3(transform * vec4(aPos, 1.0f)); // Calculate the world space of the vertex.
    normal = mat3(transpose(inverse(transform))) * aNormal; // Preserves the normal directions of the normals through object scaling (inverse) and rotation (transpose).

    gl_Position = (proj * view * transformation) * vec4(aPos, 1.0f);

    worldPos = vec3(transformation * vec4(aPos, 1.0f));
    color = aColor;
    texCoord = offset + (aTex * scale);
}