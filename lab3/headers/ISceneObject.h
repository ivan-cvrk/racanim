#pragma once

#include "Transform.h"
#include <glm/glm.hpp>

class Shader;

class ISceneObject : public Transform {
public:
  virtual Shader* getShader() = 0;
  virtual void setShader(Shader *shader) = 0;
  virtual Shader* getDepthShader() = 0;
  virtual void setDepthShader(Shader *shader) = 0;
  virtual void render(const glm::mat4 &projectionMatrix,
                      const glm::mat4 &viewMatrix,
                      const glm::mat4 &parentModel = glm::mat4{1}) = 0;
  virtual void update([[maybe_unused]] float deltaTime) {}
};
