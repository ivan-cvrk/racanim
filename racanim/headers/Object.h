#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "ISceneObject.h"
#include "Material.h"
#include "IRenderable.h"

class Shader;

class Object : public ISceneObject {
  Shader *shader = nullptr;
  Material material;
  std::vector<IRenderable *> renderables;

public:
  Object(std::vector<IRenderable *>&& renderables, Material material)
  : material(material), renderables(renderables) {}

  Object(std::vector<IRenderable *>&& renderables)
  : material(Material{{0.3, 0.3, 0.3,}, {1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}, 32.0f}),
    renderables(renderables) {}

  virtual ~Object();

  Shader* getShader() override { return this->shader; }

  void setShader(Shader *shader) override {
    this->shader = shader;
  }

  void setMaterial(Material material) {
    this->material = material;
  }

  void render(const glm::mat4 &projectionMatrix,
              const glm::mat4 &viewMatrix,
              const glm::mat4& parentModel) override;

  void update(float deltaTime) override;
};
