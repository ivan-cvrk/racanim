#include "TriangleMesh.h"
#include <assimp/scene.h>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>


TriangleMesh::TriangleMesh(const aiMesh *mesh) {
  this->vertices.resize(mesh->mNumVertices);
  this->normals.resize(mesh->mNumVertices);
  this->indices.resize(mesh->mNumFaces * 3);

  if (mesh->HasTextureCoords(0)) {
    this->uvCoords.resize(mesh->mNumVertices);
  } else {
    this->uvCoords.resize(0);
  }

  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    this->vertices[i] = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
    this->normals[i] = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
    if (mesh->HasTextureCoords(0)) {
      this->uvCoords[i] = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
    }
  }

  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    for (unsigned int j = 0; j < 3; j++) {
      this->indices[i * 3 + j] = mesh->mFaces[i].mIndices[j];
    }
  }

  glGenVertexArrays(1, &VAO);
    glGenBuffers(3, VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * this->vertices.size(), &this->vertices[0].x, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * this->normals.size(), &this->normals[0].x, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * this->uvCoords.size(), &this->uvCoords[0].x, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void *)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * this->indices.size(), &this->indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

  glBindVertexArray(0);
}

void TriangleMesh::applyTransform(const glm::mat4 &mat) {
  for (unsigned int i = 0; i < this->vertices.size(); i++) {
    this->vertices[i] = mat * glm::vec4(this->vertices[i], 1.f);
  }

  glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * this->vertices.size(), &this->vertices[0]);
}

std::pair<glm::vec3, glm::vec3> TriangleMesh::getBoundingBox() {
  glm::vec3 min = this->vertices[0];
  glm::vec3 max = this->vertices[0];

  for (unsigned int i = 1; i < this->vertices.size(); i++) {
    min.x = std::min(min.x, this->vertices[i].x);
    min.y = std::min(min.y, this->vertices[i].y);
    min.z = std::min(min.z, this->vertices[i].z);

    max.x = std::max(max.x, this->vertices[i].x);
    max.y = std::max(max.y, this->vertices[i].y);
    max.z = std::max(max.z, this->vertices[i].z);
  }

  return {min, max};
}

void TriangleMesh::normalize() {
  auto maxmin = this->getBoundingBox();

  glm::vec3 &min = maxmin.first;
  glm::vec3 &max = maxmin.second;

  float avgX = (min.x + max.x) / 2.f;
  float avgY = (min.y + max.y) / 2.f;
  float avgZ = (min.z + max.z) / 2.f;

  float M = std::max(std::max(max.x - min.x, max.y - min.x), max.z - min.z);

  std::cout << "M = " <<  M << std::endl;
  std::cout << "Before mesh normalization: " << std::endl;
  std::cout << "min coords = " << maxmin.first.x << " " << maxmin.first.y << " " << maxmin.first.z << std::endl;
  std::cout << "max coords = " << maxmin.second.x << " " << maxmin.second.y << " " << maxmin.second.z << std::endl;

  glm::mat4 transform(1.f);
  transform = glm::scale(transform, glm::vec3{2/M, 2/M, 2/M});
  transform = glm::translate(transform, glm::vec3{-avgX, -avgY, -avgZ});

  this->applyTransform(transform);

  auto maxmin2 = this->getBoundingBox();
  std::cout << "After mesh normalization: " << std::endl;
  std::cout << "min coords = " << maxmin2.first.x << " " << maxmin2.first.y << " " << maxmin2.first.z << std::endl;
  std::cout << "max coords = " << maxmin2.second.x << " " << maxmin2.second.y << " " << maxmin2.second.z << std::endl;
}

void TriangleMesh::draw([[maybe_unused]]const Shader* shader) {
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

TriangleMesh::~TriangleMesh() {
  glDeleteBuffers(3, VBO);
  glDeleteBuffers(1, &EBO);
  glDeleteVertexArrays(1, &VAO);
}
