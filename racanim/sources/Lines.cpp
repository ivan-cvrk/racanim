#include <Lines.h>

void Lines::setColor(const glm::vec3 color) { this->color = color; }

void Lines::setColor(const glm::ivec3 color) {
  this->color.r = color.r / 255.f;
  this->color.g = color.g / 255.f;
  this->color.b = color.b / 255.f;
}

void Lines::clearGL() {
  if (this->pointsCount == 0) {
    return;
  }
  this->pointsCount = 0;
  glDeleteBuffers(2, VBO);
  glDeleteVertexArrays(1, &VAO);
}

void Lines::setPoints(const std::vector<glm::vec3> &points) {
  this->pointsCount = points.size();

  std::vector<glm::vec3> colors(points.size());
  for (size_t i = 0; i < points.size(); i++) {
    colors[i] = color;
  }

  glGenVertexArrays(1, &VAO);
  glGenBuffers(2, VBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * points.size(), &points[0].x,
               GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * colors.size(), &colors[0].x,
               GL_STATIC_DRAW);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
}

void Lines::draw([[maybe_unused]] const Shader *shader) {
  if (this->pointsCount == 0) {
    return;
  }
  if (this->pointsCount == 1) {
    glBindVertexArray(VAO);
    glPointSize(10);
    glDrawArrays(GL_POINTS, 0, pointsCount);
  }
  glLineWidth(3);
  glBindVertexArray(VAO);
  glDrawArrays(GL_LINE_STRIP, 0, pointsCount);
  glLineWidth(1);
}

Lines::~Lines() { clearGL(); }
