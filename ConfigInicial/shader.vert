#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in ivec4 aBoneIDs;
layout (location = 4) in vec4 aBoneWeights;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;
uniform mat4 uBoneMatrices[100];  // Máximo 100 huesos

out vec2 TexCoords;

void main() {
    mat4 boneTransform = 
        aBoneWeights.x * uBoneMatrices[aBoneIDs.x] +
        aBoneWeights.y * uBoneMatrices[aBoneIDs.y] +
        aBoneWeights.z * uBoneMatrices[aBoneIDs.z] +
        aBoneWeights.w * uBoneMatrices[aBoneIDs.w];

    vec4 pos = uModel * boneTransform * vec4(aPos, 1.0);
    gl_Position = uProjection * uView * pos;
    TexCoords = aTexCoords;
}