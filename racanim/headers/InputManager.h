#pragma once
#include "IMoveable.h"
#include <GLFW/glfw3.h>

namespace InputManager {

void init(GLFWwindow *window);
void registerMoveable(IMoveable *moveable);
void removeMoveable(IMoveable *moveable);

void pollEvents(float deltaTime);
}; // namespace InputManager
