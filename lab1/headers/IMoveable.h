#pragma once

enum class MoveableMovement {
  UP, DOWN, RIGHT, LEFT, FORWARD, BACKWARD
};

class IMoveable {
public:
  virtual void onMouseScroll(double yoffset) = 0;
  virtual void onMouseMove(double xpos, double ypos) = 0;
  virtual void processKeyboard(MoveableMovement movement, float deltaTime) = 0;
  virtual void mouseRightClick() = 0;
  virtual void mouseLeftClick() = 0;
protected:
  virtual ~IMoveable() {};
};
