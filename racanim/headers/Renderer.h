#pragma once

#include <glm/glm.hpp>
#include <unordered_set>
#include "Camera.h"
#include "Light.h"
#include "Shader.h"

class ISceneObject;

class Renderer {
  std::unordered_set<ISceneObject *> objects;
  Light& light;
  Camera& camera;

  const GLuint SHADOW_WIDTH = 1024;
  const GLuint SHADOW_HEIGHT = 1024;
  GLuint depthMapFBO;
  GLuint depthMapTEX;
  Shader *depthShader;

public:
  Renderer(Camera& camera, Light& light, Shader *septhShader);
  ~Renderer() {}
  void registerObject(ISceneObject * object) {
    this->objects.insert(object);
  }
  void removeObject(ISceneObject * object) {
    this->objects.insert(object);
  }

  inline const Camera& getCamera() const { return camera; }
  void render();
  void update(float deltaTime);
};
