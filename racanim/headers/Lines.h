#pragma once

#include "IRenderable.h"
#include <glm/glm.hpp>
#include <vector>

class Lines : public IRenderable {
  GLuint VBO[2];
  glm::vec3 color = {57/255.0f, 99/255.0f, 186/255.0f};
  GLuint pointsCount = 0;

  void clearGL();
public:
  ~Lines();
  void setColor(glm::vec3);
  void setColor(glm::ivec3);
  void setPoints(const std::vector<glm::vec3>& points);
  virtual void draw() override;
};
