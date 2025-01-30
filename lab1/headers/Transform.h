#pragma once

#include "glm/ext/matrix_transform.hpp"
#include "MyGLM.h"

class Transform {
private:
  glm::vec3 Position;
  glm::vec3 Front;
  glm::vec3 Up;
  glm::vec3 Right;
  glm::vec3 Scale;

  double Yaw, Pitch;

  void updateYawPitch();

public:
  Transform();
  Transform(glm::vec3 Position, glm::vec3 Front, glm::vec3 Up, glm::vec3 Right,
            glm::vec3 Scale);

  virtual ~Transform() {};

  void setOrientation(glm::vec3 front, glm::vec3 up);
  void setModelMatrix(glm::mat4 modelMatrix);
  void localMove(glm::vec3 delta);
  void rotate(glm::mat4 rot);
  void rotateFPS(float xOffset, float yOffset, bool constrainPitch);

  glm::mat4 getModelMatrix() const;
  inline glm::mat4 getViewMatrix() const {
    return MyGLM::lookAtMatrix(Position, Position + Front, Up);
  }

  inline glm::vec3 FrontVec() const { return Front; }
  inline glm::vec3 UpVec() const { return Up; }
  inline glm::vec3 RightVec() const { return Right; }
  inline glm::vec3 PositionVec() const { return Position; }

  inline void globalMove(glm::vec3 delta) { this->Position += delta; }
  inline void scale(glm::vec3 scale) { this->Scale = scale; }
  inline void setPosition(glm::vec3 pos) { this->Position = pos; }
};
