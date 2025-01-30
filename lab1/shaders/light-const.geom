#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

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

void main() {
  vec3 p0 = gl_in[0].gl_Position.xyz / gl_in[0].gl_Position.w;
  vec3 p1 = gl_in[1].gl_Position.xyz / gl_in[1].gl_Position.w;
  vec3 p2 = gl_in[2].gl_Position.xyz / gl_in[2].gl_Position.w;

  vec3 normal = normalize(cross(p2-p1, p0-p1));

  vec3 middle = (p0 + p1 + p2) / 3.0;


 // Ambient lighting
  vec3 ambient = LightAmbient * MaterialAmbient;

  vec3 lightDir = normalize(LightPosition - middle);
  vec3 camDir = normalize(CameraPosition - middle);

  // Diffuse shading
  float diff = max(dot(normal, lightDir), 0.0);
  vec3 diffuse = LightDiffuse * MaterialDiffuse * diff;

  // Speculat shading
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(camDir, reflectDir), 0.0), LightExponent);
  vec3 specular = LightSpecular * MaterialSpecular * spec;

  float distance = length(LightPosition - middle);
  float attenuation = 1.0 / (1.0 + distance * 0.01);

  Color = ambient + (diffuse + specular) * attenuation;
  for (int i = 0; i < 3; ++i) {
      gl_Position = projection * view * gl_in[i].gl_Position;
      EmitVertex();
  }

  EndPrimitive();
}
