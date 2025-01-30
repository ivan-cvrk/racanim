#pragma once

#include "ISceneObject.h"
#include "Transform.h"
#include <vector>

class InstancedObjectDecorator : public ISceneObject {
  ISceneObject *baseInstance;
  std::vector<Transform> instances;

public:
  InstancedObjectDecorator(ISceneObject *baseInstance)
      : baseInstance(baseInstance) {}

  inline void addInstance(const Transform& transform) {
    instances.push_back(transform);
  }

  virtual Shader *getShader() override;
  virtual void setShader(Shader *shader) override;
  virtual void render(const glm::mat4 &projectionMatrix,
                      const glm::mat4 &viewMatrix,
                      const glm::mat4 &parentModel) override;
};
