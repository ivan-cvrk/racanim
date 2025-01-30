#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

uniform vec3 LightAmbient;
uniform vec3 LightDiffuse;
uniform vec3 LightSpecular;
uniform vec3 LightPosition;
uniform vec3 CameraPosition;

uniform float LightExponent;
uniform vec3 MaterialAmbient;
uniform vec3 MaterialDiffuse;
uniform vec3 MaterialSpecular;

out vec3 Color;

void main()
{
  vec4 position = projection * view * model * vec4(aPos, 1.0);
  vec3 positionWorkingSpace = position.xyz / position.w;
  vec3 normal = normalize(mat3(transpose(inverse(model))) * aNormal);

  // Ambient lighting
  vec3 ambient = LightAmbient * MaterialAmbient;

  vec3 lightDir = normalize(LightPosition - positionWorkingSpace);
  vec3 camDir = normalize(CameraPosition - positionWorkingSpace);

  // Diffuse shading
  float diff = max(dot(normal, lightDir), 0.0);
  vec3 diffuse = LightDiffuse * MaterialDiffuse * diff;

  // Speculat shading
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(camDir, reflectDir), 0.0), LightExponent);
  vec3 specular = LightSpecular * MaterialSpecular * spec;

  float distance = length(LightPosition - positionWorkingSpace);
  float attenuation = 1.0 / (1.0 + distance * 0.01);

  Color = ambient + (diffuse + specular) * attenuation;
  gl_Position = position;
}
