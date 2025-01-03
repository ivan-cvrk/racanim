#pragma once

#include "IRenderable.h"
#include <glm/glm.hpp>
#include <utility>
#include <vector>

class aiMesh;

class TriangleMesh : public IRenderable {
  std::vector<glm::vec3> vertices;
  std::vector<glm::vec3> normals;
  std::vector<glm::vec2> uvCoords;
  std::vector<GLuint> indices;

  GLuint VBO[3];
  GLuint EBO;

public:
  TriangleMesh(const aiMesh *mesh);
  virtual ~TriangleMesh();

  void applyTransform(const glm::mat4 &mat);
  std::pair<glm::vec3, glm::vec3> getBoundingBox();
  void normalize();

  virtual void draw() override;
};
