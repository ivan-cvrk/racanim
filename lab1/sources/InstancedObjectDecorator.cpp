#include "InstancedObjectDecorator.h"

Shader *InstancedObjectDecorator::getShader() {
  return baseInstance->getShader();
}

void InstancedObjectDecorator::setShader(Shader *shader) {
  baseInstance->setShader(shader);
};

void InstancedObjectDecorator::render(const glm::mat4 &projectionMatrix,
                                      const glm::mat4 &viewMatrix,
                                      const glm::mat4 &parentModel) {
  glm::mat4 modelMatrix = parentModel * this->getModelMatrix();
  for (Transform &instance : instances) {
    glm::mat4 instanceModelMatrix = modelMatrix * instance.getModelMatrix();
    baseInstance->render(projectionMatrix, viewMatrix, instanceModelMatrix);
  }
}
