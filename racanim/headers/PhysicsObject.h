#pragma once

#include "Object.h"
#include "PhysicsEngine.h"

class Ball : public ISceneObject {
  PhysicsEngine &physicsEngine;
  Object *object;

public:
  Ball(Object *object, PhysicsEngine &physicsEngine)
      : physicsEngine(physicsEngine), object(object) {}

  void render(const glm::mat4 &projectionMatrix, const glm::mat4 &viewMatrix,
              const glm::mat4 &parentModel) override;
  Shader *getShader() override { return object->getShader(); }

  void setShader(Shader *shader) override { object->setShader(shader); };

  void update(float deltaTime) override { object->update(deltaTime); }
};

class ClothMesh : public IRenderable {
  PhysicsEngine &physicsEngine;
  int vertices;
  int indices;

  GLuint pointsVBO[1];
  GLuint pointsVAO;
  GLuint VAO;
  GLuint EBO;

  bool points;

public:
  ClothMesh(PhysicsEngine &physicsEngine, bool points)
      : physicsEngine(physicsEngine) {
    this->points = points;
    Cloth *cloth = physicsEngine.getCloth();
    std::vector<glm::vec3> vertices;

    this->vertices = cloth->vertices.size();

    glGenBuffers(1, pointsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, pointsVBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(),
                 &vertices[0].x, GL_DYNAMIC_DRAW);

    if (points) {
      glGenVertexArrays(1, &pointsVAO);
      glBindVertexArray(pointsVAO);
      glBindBuffer(GL_ARRAY_BUFFER, pointsVBO[0]);

      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3),
                            (void *)0);
      update(0);

      glEnableVertexAttribArray(0);
      glBindVertexArray(0);
    } else {
      // VAO
      this->indices = cloth->indices.size();
      glGenVertexArrays(1, &VAO);
      glGenBuffers(1, &EBO);

      glBindVertexArray(VAO);

      glBindBuffer(GL_ARRAY_BUFFER, pointsVBO[0]);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3),
                            (void *)0);
      update(0);

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                   sizeof(GLuint) * cloth->indices.size(), &cloth->indices[0],
                   GL_STATIC_DRAW);
      glEnableVertexAttribArray(0);
      glBindVertexArray(0);
    }
  }

  virtual ~ClothMesh() {
    glDeleteBuffers(1, pointsVBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &pointsVAO);
    glDeleteVertexArrays(1, &VAO);
  }

  virtual void update([[maybe_unused]] float deltaTime) override {
    Cloth *cloth = physicsEngine.getCloth();

    glBindBuffer(GL_ARRAY_BUFFER, pointsVBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * cloth->vertices.size(),
                 &cloth->vertices[0].x, GL_DYNAMIC_DRAW);
  }

  virtual void draw([[maybe_unused]]const Shader* shader) override {
    if (points) {
      glPointSize(5);
      glBindVertexArray(pointsVAO);
      glDrawArrays(GL_POINTS, 0, vertices);
    } else {
      glBindVertexArray(VAO);
      glDrawElements(GL_TRIANGLES, this->indices, GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);
  };
};
