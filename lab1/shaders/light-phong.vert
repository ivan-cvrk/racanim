#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

uniform mat4 LightSpaceMatrix;

out vec4 FragPosLightSpace;
out vec3 Normal;
out vec3 FragPos;

void main()
{
  Normal = mat3(transpose(inverse(model))) * aNormal;

  FragPos = vec3(model * vec4(aPos, 1.0));
  FragPosLightSpace = LightSpaceMatrix * vec4(FragPos, 1.0);
  gl_Position = projection * view * model * vec4(aPos, 1.0);
}
