#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform vec3 LightAmbient;
uniform vec3 LightDiffuse;
uniform vec3 LightSpecular;
uniform vec3 LightPosition;
uniform vec3 LightDirection;
uniform float LightCutoff;
uniform float LightOuterCutoff;
uniform float LightExponent;

uniform vec3 CameraPosition;

uniform sampler2D texture_diffuse;
uniform vec3 MaterialAmbient;
uniform vec3 MaterialSpecular;

void main()
{
  vec3 normal = normalize(Normal);
  vec3 fragColor = vec3(texture(texture_diffuse, TexCoords));

  // Ambient lighting
  vec3 ambient = LightAmbient * fragColor;

  vec3 color = ambient;

  vec3 fragToLightDir = normalize(LightPosition - FragPos);

  float theta = dot(fragToLightDir, -LightDirection);
  float epsilon = LightCutoff - LightOuterCutoff;
  float intensity = clamp((theta - LightOuterCutoff) / epsilon, 0.0, 1.0);

  vec3 camDir = normalize(CameraPosition - FragPos);

  // Diffuse shading
  float diff = max(dot(normal, fragToLightDir), 0.0);
  vec3 diffuse = LightDiffuse * fragColor * intensity * diff;

  // Speculat shading
  vec3 reflectDir = reflect(-fragToLightDir, normal);
  float spec = pow(max(dot(camDir, reflectDir), 0.0), LightExponent);
  vec3 specular = LightSpecular * MaterialSpecular * intensity * spec;

  float distance = length(LightPosition - FragPos);
  float attenuation = 1.0 / (1.0 + distance * 0.01);

  color = color + (diffuse + specular) * attenuation;

  FragColor = vec4(color, 1.0);
}

