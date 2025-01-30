#pragma once

#include <glm/glm.hpp>

class MyGLM {
public:
  static glm::mat4 translate3D(glm::vec3 translateVector);
  static glm::mat4 scale3D(glm::vec3 scaleVector);
  static glm::mat4 rotate(glm::vec3 axis, float angle);
  static glm::mat4 lookAtMatrix(glm::vec3 eye, glm::vec3 center, glm::vec3 viewUp);
  static glm::mat4 frustum(float l, float r , float b, float t , float n, float f );
};
