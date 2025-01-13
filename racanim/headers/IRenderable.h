#pragma once

#include "Shader.h"
#include <glad/glad.h>

class IRenderable {
protected:
  GLuint VAO;
public:
  virtual void draw(const Shader* shader) = 0;
  virtual ~IRenderable() {}
  virtual void update([[maybe_unused]] float deltaTime) {}
};
