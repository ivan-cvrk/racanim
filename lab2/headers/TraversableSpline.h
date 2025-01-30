#include "ISpline.h"
#include "IRenderable.h"
#include "Lines.h"

class TraversableSpline : public IRenderable {
  glm::vec3 position;
  glm::vec3 tangent;
  glm::vec3 normal;
  glm::vec3 bitangent;
  ISpline* spline;
  Lines tangentLine;
  Lines bitangentLine;
  Lines normalLine;
public:
  TraversableSpline(ISpline* spline): spline(spline) {
    tangentLine.setColor(glm::vec3(1.f, 0.f, 0.f));
    normalLine.setColor(glm::vec3(0.f, 1.f, 0.f));
    bitangentLine.setColor(glm::vec3(0.f, 0.f, 1.f));
  }
  void setPoint(float t);
  void draw() {
    tangentLine.draw();
    normalLine.draw();
    bitangentLine.draw();
  };

  inline glm::vec3 Position() { return position; }
  inline glm::vec3 Tangent() { return tangent; }
  inline glm::vec3 Normal() { return normal; }
  inline glm::vec3 Bitangent() { return bitangent; }
};
