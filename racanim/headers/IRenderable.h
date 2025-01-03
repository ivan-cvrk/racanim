#pragma once

#include <glad/glad.h>

class IRenderable {
public:
  GLuint VAO;
  virtual void draw() = 0;
  virtual ~IRenderable() {}
};
