#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <string>

class Texture {
  GLuint id;
  std::string path;
  std::string type;

public:
  Texture(const std::string& path);
  ~Texture();

  GLuint ID() { return id; }
  const std::string& getType() { return type; }
  const std::string& getPath() { return path; }
};

struct Material {
  glm::vec3 ambientColor;
  glm::vec3 diffuseColor;
  glm::vec3 specularColor;
  float specularExponent;
  Texture *diffuseTexture = nullptr;
};
