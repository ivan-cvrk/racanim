#pragma once

#include "IMoveableTransform.h"
#include <glm/gtc/type_ptr.hpp>

class Camera : public IMoveableTransform {
  int Width = 0, Height = 0;
  double mouseX = 0.0, mouseY = 0.0;
  float Zoom = 45.0f;
  float MouseSensitivity = 0.1f;
  float MovementSpeed = 0.9f;

  glm::mat4 ProjectionMatrix = glm::mat4(1);

  const float zNear = 0.1f;
  const float zFar = 100.0f;

  inline void updateProjectionMatrix() {
    const float fH = tan(glm::radians(Zoom/2)) * zNear;
    const float fW = fH * Width / (float)Height;
    this->ProjectionMatrix = MyGLM::frustum(-fW, fW, -fH, fH, zNear, zFar);
  }


public:
  Camera(int width, int height);

  virtual void processKeyboard(MoveableMovement movement, float deltaTime) override;
  virtual void onMouseScroll(double yoffset) override;
  virtual void onMouseMove(double xpos, double ypos) override;
  virtual void mouseRightClick() override { }
  virtual void mouseLeftClick() override { }

  inline void updateWidthHeight(int width, int height) {
    this->Width = width;
    this->Height = height;
    this->updateProjectionMatrix();
  }

  inline int getWidth() {
    return Width;
  }

  inline int getHeight() {
    return Height;
  }

  inline glm::mat4 getProjectionMatrix() const {
    return this->ProjectionMatrix;
  }

  void update(float deltaTime);
};

