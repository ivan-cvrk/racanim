#include "Object.h"
#include "IRenderable.h"
#include "Shader.h"
#include <glad/glad.h>

void Object::render(const glm::mat4 &projectionMatrix, const glm::mat4 &view, const glm::mat4& parentModel) {
  if (shader != nullptr) {
    shader->use();
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "projection"), 1,
                       GL_FALSE, &projectionMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "view"), 1, GL_FALSE,
                       &view[0][0]);
    glm::mat4 modelMat = parentModel * this->getModelMatrix();
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "model"), 1, GL_FALSE, &modelMat[0][0]);

    glUniform3f(glGetUniformLocation(shader->ID, "MaterialAmbient"),
                material.ambientColor.x, material.ambientColor.y, material.ambientColor.z);
    glUniform3f(glGetUniformLocation(shader->ID, "MaterialDiffuse"),
                material.diffuseColor.x, material.diffuseColor.y, material.diffuseColor.z);
    glUniform3f(glGetUniformLocation(shader->ID, "MaterialSpecular"),
                material.specularColor.x, material.specularColor.y, material.specularColor.z);
    glUniform1f(glGetUniformLocation(shader->ID, "LightExponent"),
                material.specularExponent);

    if (material.diffuseTexture != nullptr) {
      glActiveTexture(GL_TEXTURE1);
      glUniform1i(glGetUniformLocation(shader->ID, "texture_diffuse"), 1);
      glBindTexture(GL_TEXTURE_2D, material.diffuseTexture->ID());
    }
  }

  for (auto renderable : renderables) {
    renderable->draw();
  }
}

void Object::update(float deltaTime) {
  for (IRenderable* renderable : renderables) {
    renderable->update(deltaTime);
  }
}

Object::~Object() {
  for (IRenderable* renderable : renderables) {
    delete renderable;
  }
}
