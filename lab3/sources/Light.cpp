#include "Light.h"

Light::Light(const glm::vec3 &ambientIntensity,
             const glm::vec3 &diffuseIntensity,
             const glm::vec3 &specularIntensity,
             float cutoff, float outerCutoff) {
  this->ambientIntensity = ambientIntensity;
  this->diffuseIntensity = diffuseIntensity;
  this->specularIntensity = specularIntensity;
  this->cutoff = glm::radians(cutoff);
  this->outerCutoff = glm::radians(outerCutoff);
}

void Light::initializeUniforms(GLuint shaderID) {
  glUniform3f(glGetUniformLocation(shaderID, "LightAmbient"),
              ambientIntensity.x, ambientIntensity.y, ambientIntensity.z);
  glUniform3f(glGetUniformLocation(shaderID, "LightDiffuse"),
              diffuseIntensity.x, diffuseIntensity.y, diffuseIntensity.z);
  glUniform3f(glGetUniformLocation(shaderID, "LightSpecular"),
              specularIntensity.x, specularIntensity.y, specularIntensity.z);
  const glm::vec3 &lightPosition = PositionVec();
  glUniform3f(glGetUniformLocation(shaderID, "LightPosition"),
              lightPosition.x, lightPosition.y, lightPosition.z);
  const glm::vec3 &lightDirection = FrontVec();
  glUniform3f(glGetUniformLocation(shaderID, "LightDirection"),
              lightDirection.x, lightDirection.y, lightDirection.z);
  glUniform1f(glGetUniformLocation(shaderID, "LightCutoff"), cos(cutoff));
  glUniform1f(glGetUniformLocation(shaderID, "LightOuterCutoff"), cos(outerCutoff));
}
