#include <PhysicsObject.h>

void Ball::render(const glm::mat4 &projectionMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &parentModel) {
  for (auto sphere: physicsEngine.spheres) {
    this->object->setPosition(sphere->position);
    this->object->scale(glm::vec3(sphere->radius, sphere->radius, sphere->radius));
    this->object->render(projectionMatrix, viewMatrix, parentModel);
  }
}
