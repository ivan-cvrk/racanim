#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec4 FragPosLightSpace;

uniform vec3 LightAmbient;
uniform vec3 LightDiffuse;
uniform vec3 LightSpecular;
uniform vec3 LightPosition;
uniform vec3 LightDirection;
uniform float LightCutoff;
uniform float LightOuterCutoff;
uniform float LightExponent;

uniform vec3 CameraPosition;

uniform vec3 MaterialAmbient;
uniform vec3 MaterialDiffuse;
uniform vec3 MaterialSpecular;

uniform sampler2D depth_texture;

float CalculateShadow(vec4 fragPosLightSpace) {
  float bias = 0.001;
  vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
  projCoords = projCoords * 0.5 + 0.5;
  float closestDepth = texture(depth_texture, projCoords.xy).r + bias;
  float currentDepth = projCoords.z;

  float shadow = currentDepth > closestDepth ? 1.0 : 0.0;

  vec2 texelSize = 1.0 / textureSize(depth_texture, 0);
  for(int x = -1; x <= 1; ++x)
  {
      for(int y = -1; y <= 1; ++y)
      {
          float pcfDepth = texture(depth_texture, projCoords.xy + vec2(x, y) * texelSize).r; 
          shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
      }
  }
  shadow /= 9.0;

  return shadow;
}

void main()
{
  vec3 normal = normalize(Normal);

  // Ambient lighting
  vec3 ambient = MaterialAmbient * LightAmbient;
  vec3 color = ambient;

  vec3 fragToLightDir = normalize(LightPosition - FragPos);

  float theta = dot(fragToLightDir, -LightDirection);
  float epsilon = LightCutoff - LightOuterCutoff;
  float intensity = theta;//clamp((theta - LightOuterCutoff) / epsilon, 0.0, 1.0);

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

  float shadow = CalculateShadow(FragPosLightSpace);
  color = color + (1.0 - shadow) * (diffuse + specular) * attenuation;

  FragColor = vec4(color, 1.0);
}

