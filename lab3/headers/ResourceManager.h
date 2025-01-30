#pragma once

#include "Object.h"
#include "Shader.h"
#include "Material.h"

#include "assimp/material.h"
#include <unordered_map>
#include <string>

class ResourceManager {
private:
  const char *const execpath;
  std::unordered_map<std::string, Object *> objects;
  std::unordered_map<std::string, Shader *> shaders;
  std::unordered_map<std::string, Texture *> textures;

  Texture* getTexture(const char* texturePath);
  Material loadMaterial(const char* scenePath, const aiMaterial* material);
public:
  ResourceManager(const char *execpath);

  Material getMaterial(const char* name, unsigned int materialID);
  Object* getScene(const char *name);
  Shader* getShader(const char *name, const char *vertName, const char *fragName, const char *geomName = nullptr);

  ~ResourceManager();
};
