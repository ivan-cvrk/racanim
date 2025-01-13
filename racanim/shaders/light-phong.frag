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
    // Calculate projection coordinates
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    // Retrieve depth and bias
    float bias = 0.005;
    float closestDepth = texture(depth_texture, projCoords.xy).r + bias;
    float currentDepth = projCoords.z;

    // If outside the shadow map, no shadow
    if (projCoords.z > 1.0) 
        return 0.0;

    // Stronger distance-based kernel size
    float distance = length(FragPos - LightPosition);
    float kernelRadius = clamp(distance * distance * 0.3 - distance * 0.1, 2.0, 3.0); // Larger scaling factor for more blur
    vec2 texelSize = kernelRadius / textureSize(depth_texture, 0);

    // Percentage Closer Filtering (PCF)
    float shadow = 0.0;
    int samples = int(kernelRadius) * 2 + 1; // Increase the sample count dynamically
    for (int x = -samples / 2; x <= samples / 2; ++x) {
        for (int y = -samples / 2; y <= samples / 2; ++y) {
            float pcfDepth = texture(depth_texture, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }

    // Normalize by the number of samples
    shadow /= float(samples * samples);

    //shadow = 0.0;
    //totalWeight = 0.0;
    //for (int x = -samples / 2; x <= samples / 2; ++x) {
    //    for (int y = -samples / 2; y <= samples / 2; ++y) {
    //        vec2 offset = vec2(x, y) * texelSize;
    //        float weight = exp(-(x * x + y * y) / (2.0 * kernelRadius)); // Gaussian weight
    //        float pcfDepth = texture(depth_texture, projCoords.xy + offset).r;
    //        shadow += weight * (currentDepth - bias > pcfDepth ? 1.0 : 0.0);
    //        totalWeight += weight;
    //    }
    //}
    //shadow /= totalWeight; // Normalize by total weight

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
  float intensity = clamp((theta - LightOuterCutoff) / epsilon, 0.0, 1.0);

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

