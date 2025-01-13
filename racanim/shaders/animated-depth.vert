
#version 330 core
layout (location = 0) in vec3 position; 
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec4 boneIds;
layout (location = 4) in vec4 boneWeights;

uniform mat4 LightSpaceMatrix;
uniform mat4 model;

uniform mat4 bone_transforms[50];

void main()
{
  mat4 boneTransform  =  mat4(0.0);
  boneTransform  +=    bone_transforms[int(boneIds.x)] * boneWeights.x;
  boneTransform  +=    bone_transforms[int(boneIds.y)] * boneWeights.y;
  boneTransform  +=    bone_transforms[int(boneIds.z)] * boneWeights.z;
  boneTransform  +=    bone_transforms[int(boneIds.w)] * boneWeights.w;
  vec4 pos = boneTransform * vec4(position, 1.0);
  gl_Position = LightSpaceMatrix * model * pos;
}
