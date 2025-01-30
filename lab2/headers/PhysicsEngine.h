#pragma once
#include "glm/geometric.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <iostream>

const float GRAVITY = 9.81f/2;
const float DRAG = 0.001f;

enum class ObjectType { Sphere, Plane, Cloth };

class PhysicsObject {
public:
  virtual ~PhysicsObject() = default;
  virtual ObjectType getType() const = 0;
  virtual void update(float deltaTime) = 0;
};

class Sphere : public PhysicsObject {
public:
  glm::vec3 position;
  glm::vec3 velocity;
  float radius;
  float mass;
  bool stationary = false;

  Sphere(glm::vec3 position, glm::vec3 velocity, float radius, float mass)
      : position(position), velocity(velocity), radius(radius), mass(mass) {}

  void update(float deltaTime) override {
    if (stationary) {
      return;
    }

    velocity += (accumulatedForces / mass) * deltaTime;
    if (position.y <= radius) {
      position.y = radius;
      velocity -= velocity * DRAG * 0.5f;
      velocity.y = 0;
    }
    if (glm::length(velocity) == 0.0f) {
      stationary = true;
    }
    position += velocity * deltaTime;
    resetForces();
  }

  void applyForce(const glm::vec3 &force) { accumulatedForces += force; }

  ObjectType getType() const override { return ObjectType::Sphere; }

  void resetForces() { accumulatedForces = glm::vec3(0, -GRAVITY * mass, 0); }

private:
  glm::vec3 accumulatedForces;
};

class Plane : public PhysicsObject {
public:
  glm::vec3 normal;
  float distance; // Distance from the origin along the normal

  void update(float deltaTime) override {
    // Planes are static, so no update needed
  }

  ObjectType getType() const override { return ObjectType::Plane; }
};

struct BoundingBox {
  glm::vec3 max;
  glm::vec3 min;

  bool isInside(glm::vec3 position, float fault) {
    return (position.x + fault >= min.x && position.x - fault <= max.x && position.y + fault >= min.y &&
            position.y - fault <= max.y && position.z + fault >= min.z && position.z - fault <= max.z);
  }
};

class Cloth : public PhysicsObject {
public:
  const int HORIZONTAL = 20;
  const int VERTICAL = 20;
  const float SPRING_LENGTH = 0.05f;
  const float SHEAR_SPRING_LENGTH = SPRING_LENGTH * sqrt(2);
  const float FLEXION_SPRING_LENGTH = SPRING_LENGTH * 2.f;
  const float MASS = 0.005f;
  const float DRAG = 0.1f;
  const float SPRING_K = 50;
private:
  std::vector<bool> fixed;
  std::vector<glm::vec3> lastVertices;

  void updateConstraints(float deltaTime);

public:
  std::vector<glm::vec3> forces;
  std::vector<unsigned int> indices;
  std::vector<glm::vec3> vertices;

  Cloth(glm::mat4 transrom, int horizontal, int vertical, float springLength, float mass);
  virtual ~Cloth();

  BoundingBox getBoundingBox();
  void update(float deltaTime) override;
  ObjectType getType() const override { return ObjectType::Cloth; };
};

class PhysicsEngine {
public:
  std::vector<std::shared_ptr<Sphere>> spheres;
  Cloth *cloth;

  PhysicsEngine() {
    glm::mat4 transform(1);
    transform = glm::translate(transform, {0, 1.5f, 0});
    cloth = new Cloth(transform, 20, 20, 0.05, 0.01);
  }

  void addSphere(glm::vec3 position, glm::vec3 velocity, float radius,
                 float mass) {
    spheres.push_back(
        std::make_shared<Sphere>(position, velocity, radius, mass));
  }

  void update(float deltaTime) {
    //auto condition = [](const std::shared_ptr<Sphere> &sphere) {
    //  return sphere->position.y < 0.0f;
    //};

    //spheres.erase(std::remove_if(spheres.begin(), spheres.end(), condition),
    //              spheres.end());

    const float targetFPS = 0.001f;
    const int steps = deltaTime / targetFPS;
    deltaTime = targetFPS;
    for (int i = 0; i < steps; i++) {
      cloth->update(deltaTime);
      for (auto &object : spheres) {
        object->update(deltaTime);
      }
      resolveCollisions(deltaTime);
    }
  }

  Cloth *getCloth() { return cloth; }

  void resolveCollisions(float deltaTime) {
    BoundingBox bbox = cloth->getBoundingBox();
    for (size_t i = 0; i < spheres.size(); ++i) {
      if (bbox.isInside(spheres[i]->position, spheres[i]->radius)) {
        resolveSphereClothCollision(*cloth, *spheres[i], deltaTime);
      }
    }
  }

private:
  void handleCollision(PhysicsObject &a, PhysicsObject &b, float deltaTime) {
    std::cout << "handling collision" << std::endl;
    if (a.getType() == ObjectType::Cloth && b.getType() == ObjectType::Sphere) {
      resolveSphereClothCollision(static_cast<Cloth &>(a),
                                  static_cast<Sphere &>(b), deltaTime);
    }

    if (a.getType() == ObjectType::Sphere && b.getType() == ObjectType::Plane) {
      resolveSpherePlaneCollision(static_cast<Sphere &>(a),
                                  static_cast<Plane &>(b));
    }

    if (a.getType() == ObjectType::Cloth && b.getType() == ObjectType::Plane) {
      resolveClothPlaneCollision(static_cast<Cloth &>(a),
                                 static_cast<Plane &>(b));
    }
  }

  void resolveSphereClothCollision(Cloth &cloth, Sphere &sphere, float deltaTime) {

    for (int i = 0; i < cloth.VERTICAL; i++) {
      for (int j = 0; j < cloth.HORIZONTAL; j++) {
        const int index = i * cloth.HORIZONTAL + j;

        glm::vec3 directionToSphere = cloth.vertices[index] - sphere.position;
        float distance = glm::length(directionToSphere);

        // Check if the particle is inside the sphere
        if (distance < sphere.radius) {
            // Correct the particle's position
            glm::vec3 correction = glm::normalize(directionToSphere) * (sphere.radius - distance);
            cloth.vertices[index] += correction;

            // Calculate reaction force
            glm::vec3 reactionForce = -correction / (deltaTime * deltaTime);

            // Apply force to the sphere
            //sphere.applyForce(reactionForce);

            // Apply repelling force to the particle
            cloth.forces[index] -= reactionForce * 0.01f;
        }

      }
    }
  }

  void resolveSpherePlaneCollision(Sphere &sphere, Plane &plane) {}

  void resolveClothPlaneCollision(Cloth &cloth, Plane &plane) {}
};
