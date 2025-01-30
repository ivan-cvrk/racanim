#include "PhysicsEngine.h"
#include <iostream>

Cloth::Cloth(glm::mat4 transform, int horizontal, int vertical, float springLength, float mass)
    : HORIZONTAL(horizontal), VERTICAL(vertical), SPRING_LENGTH(springLength), MASS(mass) {
  this->vertices = std::vector<glm::vec3>(VERTICAL * HORIZONTAL);
  this->fixed.resize(VERTICAL * HORIZONTAL);
  this->forces.resize(VERTICAL * HORIZONTAL);

  for (int i = 0; i < VERTICAL; i++) {
    for (int j = 0; j < HORIZONTAL; j++) {
      vertices[i * HORIZONTAL + j] =
          transform * glm::vec4((SPRING_LENGTH-0.1f) * j, -SPRING_LENGTH * i, 0, 1);
      fixed[i * HORIZONTAL + j] = false;
      forces[i * HORIZONTAL + j] = glm::vec3(0, 0, 0);
    }
  }
  // Step 2: Generate the indices for the triangle topology
  for (int i = 0; i < vertical - 1; i++) {
    for (int j = 0; j < horizontal - 1; j++) {
      // Compute the indices of the four corners of the current quad
      int topLeft = i * horizontal + j;
      int topRight = topLeft + 1;
      int bottomLeft = (i + 1) * horizontal + j;
      int bottomRight = bottomLeft + 1;

      // First triangle
      indices.push_back(topLeft);
      indices.push_back(bottomLeft);
      indices.push_back(topRight);

      // Second triangle
      indices.push_back(topRight);
      indices.push_back(bottomLeft);
      indices.push_back(bottomRight);
    }
  }

  //fixed[HORIZONTAL / 2] = true;
  fixed[0] = true;
  fixed[HORIZONTAL - 1] = true;
  this->lastVertices = this->vertices;
}

inline glm::vec3 calculateStructuralSpringForce(glm::vec3 currentPos,
                                                glm::vec3 neighbour,
                                                float springLength,
                                                float springConst) {
  glm::vec3 diff = currentPos - neighbour;
  float length = glm::length(diff);
  return -springConst * (length - springLength) / length * diff;
}

inline glm::vec3 calculateShearSpringForce(glm::vec3 currentPos,
                                           glm::vec3 neighbour,
                                           float springLength,
                                           float springConst) {
  glm::vec3 diff = currentPos - neighbour;
  float length = glm::length(diff);
  return -springConst * (length - springLength) / length * diff;
}

inline glm::vec3 calculateFlexionSpringForce(glm::vec3 currentPos,
                                             glm::vec3 neighbour,
                                             float springLength,
                                             float springConst) {
  glm::vec3 diff = currentPos - neighbour;
  float length = glm::length(diff);
  float spring_k = springConst;
  if (length > springLength) {
    spring_k = (1 + (springLength / length)) * springConst;
  }
  return -spring_k * (length - springLength) / length * diff;
}

void Cloth::update(float deltaTime) {
  updateConstraints(deltaTime);
}

void Cloth::updateConstraints(float deltaTime) {
  std::vector<glm::vec3> nextVertices(vertices.size());

  // Update vertices

  glm::vec3 gravity = GRAVITY * MASS * glm::vec3(0, -1, 0);

  for (int i = 0; i < VERTICAL; i++) {
    for (int j = 0; j < HORIZONTAL; j++) {
      int current = i * HORIZONTAL + j;

      if (fixed[current]) {
        nextVertices[current] = vertices[current];
        continue;
      }

      glm::vec3 currentPos = vertices[current];

      int up = (i - 1) * HORIZONTAL + j;
      int down = (i + 1) * HORIZONTAL + j;
      int left = i * HORIZONTAL + j - 1;
      int right = i * HORIZONTAL + j + 1;

      glm::vec3 force{0, 0, 0};

      // up
      if (up >= 0) {
        force += calculateStructuralSpringForce(currentPos, vertices[up],
                                                SPRING_LENGTH, SPRING_K);
      }
      // down
      if (down < HORIZONTAL * VERTICAL) {
        force += calculateStructuralSpringForce(currentPos, vertices[down],
                                                SPRING_LENGTH, SPRING_K);
      }
      // left
      if (j - 1 >= 0) {
        force += calculateStructuralSpringForce(currentPos, vertices[left],
                                                SPRING_LENGTH, SPRING_K);
      }
      // right
      if (j + 1 < HORIZONTAL) {
        force += calculateStructuralSpringForce(currentPos, vertices[right],
                                                SPRING_LENGTH, SPRING_K);
      }

      int upup = (i - 2) * HORIZONTAL + j;
      int downdown = (i + 2) * HORIZONTAL + j;
      int leftleft = i * HORIZONTAL + j - 2;
      int rightright = i * HORIZONTAL + j + 2;

      // upup
      if (upup >= 0) {
        force += calculateFlexionSpringForce(currentPos, vertices[upup],
                                             FLEXION_SPRING_LENGTH, SPRING_K);
      }
      // downdown
      if (downdown < HORIZONTAL * VERTICAL) {
        force += calculateFlexionSpringForce(currentPos, vertices[downdown],
                                             FLEXION_SPRING_LENGTH, SPRING_K);
      }
      // leftleft
      if (j - 2 >= 0) {
        force += calculateFlexionSpringForce(currentPos, vertices[leftleft],
                                             FLEXION_SPRING_LENGTH, SPRING_K);
      }
      // rightright
      if (j + 2 < HORIZONTAL) {
        force += calculateFlexionSpringForce(currentPos, vertices[rightright],
                                             FLEXION_SPRING_LENGTH, SPRING_K);
      }

      int diagonalLeftUp = (i - 1) * HORIZONTAL + (j - 1);
      int diagonalRightUp = (i - 1) * HORIZONTAL + (j + 1);
      int diagonalLeftDown = (i + 1) * HORIZONTAL + (j - 1);
      int diagonalRightDown = (i + 1) * HORIZONTAL + (j + 1);

      // up left
      if (i > 0 && j > 0) {
        force += calculateShearSpringForce(currentPos, vertices[diagonalLeftUp],
                                           SHEAR_SPRING_LENGTH, SPRING_K);
      }
      // up right
      if (i > 0 && j < HORIZONTAL - 1) {
        force +=
            calculateShearSpringForce(currentPos, vertices[diagonalRightUp],
                                      SHEAR_SPRING_LENGTH, SPRING_K);
      }
      // down left
      if (i < VERTICAL - 1 && j > 0) {
        force +=
            calculateShearSpringForce(currentPos, vertices[diagonalLeftDown],
                                      SHEAR_SPRING_LENGTH, SPRING_K);
      }
      // down right
      if (i < VERTICAL - 1 && j < HORIZONTAL - 1) {
        force +=
            calculateShearSpringForce(currentPos, vertices[diagonalRightDown],
                                      SHEAR_SPRING_LENGTH, SPRING_K);
      }

      // gravity
      force += gravity;

      force += 0.1f * glm::vec3{0, 0, -0.5};

      // drag
      force += -this->DRAG * (currentPos - lastVertices[current]) / deltaTime;

      //force += forces[current];
      //forces[current] = glm::vec3(0, 0, 0);

      // Verlet integration
      glm::vec3 acceleration = force / MASS;
      nextVertices[current] = 2.0f * currentPos - lastVertices[current] +
                              acceleration * (deltaTime * deltaTime);
    }
  }
  lastVertices = std::move(vertices);
  vertices = std::move(nextVertices);
}

BoundingBox Cloth::getBoundingBox() {
  glm::vec3 min = this->vertices[0];
  glm::vec3 max = this->vertices[0];

  for (unsigned int i = 1; i < this->vertices.size(); i++) {
    min.x = std::min(min.x, this->vertices[i].x);
    min.y = std::min(min.y, this->vertices[i].y);
    min.z = std::min(min.z, this->vertices[i].z);

    max.x = std::max(max.x, this->vertices[i].x);
    max.y = std::max(max.y, this->vertices[i].y);
    max.z = std::max(max.z, this->vertices[i].z);
  }

  return BoundingBox{max, min};
}

Cloth::~Cloth() {}
