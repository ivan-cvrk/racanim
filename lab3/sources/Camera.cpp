#include "Camera.h"

Camera::Camera(int width, int height) {
  this->updateWidthHeight(width, height);
  this->mouseX = width / 2.0;
  this->mouseY = height / 2.0;
}

// Camera looks in the Z direction and is in right handed coordinate system,
// that's why pressing RIGHT moves the camera in the negative local X direction.
void Camera::processKeyboard(MoveableMovement movement, float deltaTime) {
  float distance = MovementSpeed * deltaTime;
  if (movement == MoveableMovement::FORWARD)
    this->localMove({0, 0, distance});
  if (movement == MoveableMovement::BACKWARD)
    this->localMove({0, 0, -distance});
  if (movement == MoveableMovement::LEFT)
    this->localMove({distance, 0, 0});
  if (movement == MoveableMovement::RIGHT)
    this->localMove({-distance, 0, 0});
  if (movement == MoveableMovement::UP)
    this->globalMove({0, distance, 0});
  if (movement == MoveableMovement::DOWN)
    this->globalMove({0, -distance, 0});
}

void Camera::onMouseScroll(double yoffset) {
  Zoom -= (float)yoffset;
  if (Zoom < 1.0f)
    Zoom = 1.0f;
  if (Zoom > 45.0f)
    Zoom = 45.0f;

  updateProjectionMatrix();
}

void Camera::onMouseMove(double xpos, double ypos) {
  double xoffset = (xpos - this->mouseX) * this->MouseSensitivity;
  double yoffset = (this->mouseY - ypos) * this->MouseSensitivity;
  this->mouseX = xpos;
  this->mouseY = ypos;

  this->rotateFPS(xoffset, yoffset, true);
}

void Camera::update(float deltaTime) {
}

