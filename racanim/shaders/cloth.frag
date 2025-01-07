#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 LightAmbient;
uniform vec3 LightDiffuse;
uniform vec3 LightSpecular;
uniform vec3 LightPosition;
uniform vec3 LightDirection;
uniform float LightExponent;

uniform vec3 CameraPosition;

uniform vec3 MaterialAmbient;
uniform vec3 MaterialDiffuse;
uniform vec3 MaterialSpecular;

uniform sampler2D depth_texture;

void main()
{
  vec3 normal = normalize(Normal);

  // Ambient lighting
  vec3 ambient = MaterialAmbient * LightAmbient;
  vec3 color = ambient * vec3(0.164, 0.778, 0.285);

  vec3 fragToLightDir = normalize(LightPosition - FragPos);

  float theta = dot(fragToLightDir, -LightDirection);
  float intensity = theta;

  vec3 camDir = normalize(CameraPosition - FragPos);

  // Diffuse shading
  float diff = max(dot(normal, fragToLightDir), 0.0);
  vec3 diffuse = LightDiffuse * MaterialDiffuse * diff * intensity;

  // Speculat shading
  vec3 reflectDir = reflect(-fragToLightDir, normal);
  float spec = pow(max(dot(camDir, reflectDir), 0.0), LightExponent);
  vec3 specular = LightSpecular * MaterialSpecular * spec * intensity;

  float distance = length(LightPosition - FragPos);
  float attenuation = 1.0 / (1.0 + distance * 0.01);

  color = color + (diffuse + specular) * attenuation;

  FragColor = vec4(color, 1.0);
}

