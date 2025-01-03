#include "InputManager.h"
#include "GLFW/glfw3.h"
#include <assert.h>

static IMoveable *moveable = nullptr;
static GLFWwindow *window = nullptr;
static float deltaTime = 0.0f;

static void mouseCallback([[maybe_unused]]GLFWwindow *window, double xpos, double ypos) {
  if (moveable != nullptr) {
    moveable->onMouseMove(xpos, ypos);
  }
}

static void scrollCallback([[maybe_unused]]GLFWwindow *window, [[maybe_unused]] double xoffset, double yoffset) {
  if (moveable != nullptr) {
    moveable->onMouseScroll(yoffset);
  }
}

void InputManager::init(GLFWwindow *window) {
  static bool initialized = false;
  assert(!initialized && "Can't initialize InputManager twice.");
  initialized = true;

  ::window = window;

  glfwSetScrollCallback(window, scrollCallback);
  glfwSetCursorPosCallback(window, mouseCallback);
}

static void sendProcessKeyboardMoveables(MoveableMovement movement) {
  if (moveable != nullptr) {
    moveable->processKeyboard(movement, ::deltaTime);
  }
}

static int mouseRightLast = GLFW_RELEASE;
static int mouseLeftLast  = GLFW_RELEASE;

void InputManager::pollEvents(float deltaTime) {
  ::deltaTime = deltaTime;
  if (glfwGetKey(::window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(::window, true);

  if (glfwGetKey(::window, GLFW_KEY_W) == GLFW_PRESS)
    sendProcessKeyboardMoveables(MoveableMovement::FORWARD);
  if (glfwGetKey(::window, GLFW_KEY_A) == GLFW_PRESS)
    sendProcessKeyboardMoveables(MoveableMovement::LEFT);
  if (glfwGetKey(::window, GLFW_KEY_S) == GLFW_PRESS)
    sendProcessKeyboardMoveables(MoveableMovement::BACKWARD);
  if (glfwGetKey(::window, GLFW_KEY_D) == GLFW_PRESS)
    sendProcessKeyboardMoveables(MoveableMovement::RIGHT);
  if (glfwGetKey(::window, GLFW_KEY_SPACE) == GLFW_PRESS)
    sendProcessKeyboardMoveables(MoveableMovement::UP);
  if (glfwGetKey(::window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    sendProcessKeyboardMoveables(MoveableMovement::DOWN);

  // mouse clicks
  if (glfwGetMouseButton(::window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
    if (mouseLeftLast != GLFW_PRESS) {
      mouseLeftLast = GLFW_PRESS;
      moveable->mouseLeftClick();
    }
  } else {
    mouseLeftLast = GLFW_RELEASE;
  }

  if (glfwGetMouseButton(::window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
    if (mouseRightLast != GLFW_PRESS) {
      mouseRightLast = GLFW_PRESS;
      moveable->mouseRightClick();
    }
  } else {
    mouseRightLast = GLFW_RELEASE;
  }
}

void InputManager::registerMoveable(IMoveable* moveable) {
  ::moveable = moveable;
}
