#include "Transform.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

Transform::Transform()
    : Position{0.0f, 0.0f, 0.0f}, Front{0.0f, 0.0f, 1.0f},
      Up{0.0f, 1.0f, 0.0f}, Right{1.0f, 0.0f, 0.0f}, Scale{1.0f, 1.0f, 1.0f},
      Yaw(90.0), Pitch(0.0){}

Transform::Transform(glm::vec3 Position, glm::vec3 Front = {0.0f, 0.0f, 1.0f},
                     glm::vec3 Up = {0.0f, 1.0f, 0.0f},
                     glm::vec3 Right = {1.0f, 0.0f, 0.0f},
                     glm::vec3 Scale = {1.0f, 1.0f, 1.0f})
    : Position(glm::normalize(Position)), Front(glm::normalize(Front)),
      Up(glm::normalize(Up)), Right(glm::normalize(Right)),
      Scale(glm::normalize(Scale)) {
  updateYawPitch();
}

glm::mat4 Transform::getModelMatrix() const {
  // Transformation matrix from local to global coordinate system
  return glm::mat4{
          Right.x,    Right.y,    Right.z,   0.0f,
             Up.x,       Up.y,       Up.z,   0.0f,
          Front.x,    Front.y,    Front.z,   0.0f,
       Position.x, Position.y, Position.z,   1.0f,
      } * MyGLM::scale3D(Scale);
}

void Transform::setModelMatrix(glm::mat4 modelMatrix) {
  this->Front = glm::normalize(modelMatrix * glm::vec4{0.0f, 0.0f, 1.0f, 0.0f});
  this->Up = glm::normalize(modelMatrix * glm::vec4{0.0f, 1.0f, 0.0f, 0.0f});
  this->Right = glm::normalize(modelMatrix * glm::vec4{1.0f, 0.0f, 0.0f, 0.0f});
  this->Position = modelMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
  updateYawPitch();
}

void Transform::setOrientation(glm::vec3 front, glm::vec3 up) {
  this->Front = glm::normalize(front);
  this->Right = -glm::normalize(glm::cross(this->Front, up));
  this->Up = glm::normalize(glm::cross(-this->Right, front));
  updateYawPitch();
}

void Transform::rotate(glm::mat4 rot) {
  this->Front = glm::normalize(rot * glm::vec4(this->Front, 0.0f));
  this->Up = glm::normalize(rot * glm::vec4(this->Up, 0.0f));
  this->Right = glm::normalize(rot * glm::vec4(this->Right, 0.0f));
  updateYawPitch();
}

void Transform::localMove(glm::vec3 delta) {
  this->Position += delta.x * this->Right;
  this->Position += delta.y * this->Up;
  this->Position += delta.z * this->Front;
}

void Transform::rotateFPS(float xOffset, float yOffset, bool constrainPitch) {
  Pitch += yOffset;
  Yaw += xOffset;

  if (constrainPitch) {
    if (Pitch > 89.0f)
      Pitch = 89.0f;
    if (Pitch < -89.0f)
      Pitch = -89.0f;
  }

  double pitch = glm::radians(Pitch);
  double yaw = glm::radians(Yaw);

  glm::vec3 direction;
  direction.x = cos(pitch) * cos(yaw);
  direction.y = sin(pitch);
  direction.z = cos(pitch) * sin(yaw);

  this->Front = glm::normalize(direction);
  this->Right = -glm::normalize(glm::cross(Front, {0.0, 1.0f, 0.0f}));
  this->Up = glm::normalize(glm::cross(-Right, Front));
}

void Transform::updateYawPitch() {
  Pitch = glm::degrees(asin(Front.y));;
  Yaw = glm::degrees(atan2(Front.z, Front.x));
}
