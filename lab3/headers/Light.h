#pragma once

#include "Transform.h"
#include <glad/glad.h>
#include <glm/glm.hpp>

class Light : public Transform {
  glm::vec3 ambientIntensity;
  glm::vec3 diffuseIntensity;
  glm::vec3 specularIntensity;
  float cutoff;
  float outerCutoff;

public:
  Light(const glm::vec3 &ambientIntensity, const glm::vec3 &diffuseIntensity,
        const glm::vec3 &specularIntensity, float cutoff = 180, float outerCutoff = 180);
  void initializeUniforms(GLuint shaderID);

  inline float getOuterCutoff() {
    return this->outerCutoff;
  }

  inline float getInnerCutoff() {
    return this->cutoff;
  }
};
