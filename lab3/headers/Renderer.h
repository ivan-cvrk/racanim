#pragma once

#include <glm/glm.hpp>
#include <unordered_set>
#include "Camera.h"
#include "Light.h"

class ISceneObject;

class Renderer {
  std::unordered_set<ISceneObject *> objects;
  Light& light;
  Camera& camera;

  const GLuint SHADOW_WIDTH = 2048;
  const GLuint SHADOW_HEIGHT = 2048;
  GLuint depthMapFBO;
  GLuint depthMapTEX;

public:
  Renderer(Camera& camera, Light& light);
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
