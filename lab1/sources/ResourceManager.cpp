#include "ResourceManager.h"

#include "TriangleMesh.h"
#include "Object.h"
#include "Shader.h"
#include "assimp/material.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <iostream>

#include <filesystem>

namespace fs = std::filesystem;

ResourceManager::ResourceManager(const char *execpath) : execpath(execpath) {}

Texture* ResourceManager::getTexture(const char* texturePath) {
  auto textureIt = textures.find(texturePath);
  if (textureIt != textures.end()) {
    return textureIt->second;
  }

  return textures[texturePath] = new Texture(texturePath);
}

Material ResourceManager::loadMaterial(const char *sceneDir, const aiMaterial* mMaterial) {
  Material material;

  aiColor4D color;
  if (AI_SUCCESS == aiGetMaterialColor(mMaterial, AI_MATKEY_COLOR_AMBIENT, &color)) {
      material.ambientColor = {color.r, color.g, color.b};
      std::cout << "Ambient Color (Ka): (" << color.r << ", " << color.g << ", " << color.b << ")" << std::endl;
  }

  // Diffuse Color (Kd)
  if (AI_SUCCESS == aiGetMaterialColor(mMaterial, AI_MATKEY_COLOR_DIFFUSE, &color)) {
      material.diffuseColor = {color.r, color.g, color.b};
      std::cout << "Ambient Color (Kd): (" << color.r << ", " << color.g << ", " << color.b << ")" << std::endl;
  }

  // Specular Color (Ks)
  if (AI_SUCCESS == aiGetMaterialColor(mMaterial, AI_MATKEY_COLOR_SPECULAR, &color)) {
     material.specularColor = {color.r, color.g, color.b};
      std::cout << "Ambient Color (Ks): (" << color.r << ", " << color.g << ", " << color.b << ")" << std::endl;
  }

  // Shininess (Ns)
  float shininess;
  if (AI_SUCCESS == aiGetMaterialFloat(mMaterial, AI_MATKEY_SHININESS, &shininess)) {
    material.specularExponent = shininess;
    std::cout << "Exponent (Ns): " << shininess << std::endl;
  }

  if (mMaterial->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
    aiString path;
    mMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path);
    fs::path resources = sceneDir;
    fs::path texturePath = resources / path.C_Str();
    std::cout << texturePath.c_str() << std::endl;
    material.diffuseTexture = getTexture(texturePath.c_str());
  }

  return material;
}

Object *ResourceManager::getScene(const char *name) {
  // if already loaded
  auto objIter = this->objects.find(name);
  if (objIter != this->objects.end()) {
    return objIter->second;
  }
  std::cout << "Loading scene: " << name << std::endl;

  fs::path executable_path = fs::canonical(fs::path(execpath));
  fs::path directory = executable_path.parent_path() / "resources";
  fs::path resource = directory / name;


  Assimp::Importer importer;
  const aiScene *scene = importer.ReadFile(resource.c_str(),
    aiProcess_CalcTangentSpace |
    aiProcess_Triangulate |
    aiProcess_JoinIdenticalVertices |
    aiProcess_SortByPType |
    aiProcess_GenSmoothNormals |
    aiProcess_FlipUVs
  );

  if (!scene) {
    std::cerr << importer.GetErrorString();
  }

  // TODO: FIX TO ITERATE OVER CHILDREN ALSO
  std::vector<IRenderable *> meshes(scene->mNumMeshes);
  for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
    TriangleMesh *pTmesh = new TriangleMesh(scene->mMeshes[i]);
    meshes[i] = pTmesh;
  }

  if (scene->mNumMaterials > 1) {
    return new Object(std::move(meshes), loadMaterial(resource.parent_path().c_str(), scene->mMaterials[1]));
  } else {
    return new Object(std::move(meshes));
  }
}

void processMesh() {

}

Shader *ResourceManager::getShader(const char *name, const char *vertName, const char *fragName, const char *geomName) {
  fs::path executable_path = fs::canonical(fs::path(execpath));
  fs::path directory = executable_path.parent_path();

  fs::path vertexFile = directory / "shaders" / vertName;
  fs::path fragmentFile = directory / "shaders" / fragName;

  Shader* shader = nullptr;
  if (geomName == nullptr) {
    shader = new Shader(vertexFile.c_str(), fragmentFile.c_str(), nullptr);
  } else {
    fs::path geomFile = directory / "shaders" / geomName;
    shader = new Shader(vertexFile.c_str(), fragmentFile.c_str(), geomFile.c_str());
  }
  shaders[name] = shader;
  return shader;
}

ResourceManager::~ResourceManager() {
  // Delete objects
  for (auto [name, object] : objects) {
    delete object;
  }

  // Delete shaders
  for (auto [name, shader] : shaders) {
    delete shader;
  }

  // Delete textures
  for (auto [name, texture] : textures) {
    delete texture;
  }
}
