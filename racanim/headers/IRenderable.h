#pragma once

#include <glad/glad.h>

class IRenderable {
protected:
  GLuint VAO;
public:
  virtual void draw() = 0;
  virtual ~IRenderable() {}
  virtual void update([[maybe_unused]] float deltaTime) {}
};
