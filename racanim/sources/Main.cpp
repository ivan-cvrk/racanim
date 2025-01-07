#include "BSplineApproximateCurve.h"
#include "BezierCurve.h"
#include "BezierManagerDecorator.h"
#include "FPSManager.h"
#include "InputManager.h"
#include "InstancedObjectDecorator.h"
#include "Object.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "Shader.h"
#include "TraversableSpline.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <random>

using namespace std;

int width = 2000, height = 1600;

void framebuffer_size_callback([[maybe_unused]] GLFWwindow *window, int Width,
                               int Height) {
  width = Width;
  height = Height;
}

Transform randomTransform() {
  static std::random_device generator;
  static std::normal_distribution<float> normal(0, 1.5);
  static std::uniform_real_distribution<float> uniform(0, 90);

  Transform transform;
  transform.globalMove({normal(generator), abs(normal(generator)) / 1.5f,
                        normal(generator) * 1.5});
  transform.rotateFPS(uniform(generator), uniform(generator), false);
  return transform;
}

int main([[maybe_unused]] int argc, char *argv[]) {
  GLFWwindow *window;

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
  window = glfwCreateWindow(width, height, "racanim", nullptr, nullptr);

  if (window == nullptr) {
    fprintf(stderr, "Failed to Create OpenGL Context");
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(window);

  gladLoadGL();
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    fprintf(stderr, "Failed to initialize GLAD");
    exit(1);
  }

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glClearColor(0.23, 0.23, 0.23, 1);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  FPSManager FPSManagerObject(window, 60, 1.0, "Zadatak X");

  fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));

  glfwSwapInterval(0);

  InputManager::init(window);

  Camera camera(width, height);
  camera.setPosition({-0.887870, 3.073193, 4.207388});
  camera.setOrientation(glm::vec3{-0.945753, -0.190809, -0.262951}, camera.UpVec());

  // ======== Curves ============
  BezierCurve *bezierCurves = new BezierCurve;
  BezierManagerDecorator *bezierManager =
      new BezierManagerDecorator(camera, *bezierCurves);
  InputManager::registerMoveable(bezierManager);

  Light light({0.2f, 0.2f, 0.2f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, 14.f,
              17.f);
  light.globalMove({-2, 3, 3});
  light.setOrientation(-light.PositionVec(), glm::vec3{0, 1, 0});

  ResourceManager *resourceManager = new ResourceManager(argv[0]);

  Shader *depthShader =
      resourceManager->getShader("depth", "depth.vert", "depth.frag");
  Renderer *renderer = new Renderer(camera, light, depthShader);

  Shader *lineShader =
      resourceManager->getShader("line", "wire.vert", "wire.frag");
  Object *curves = new Object({bezierCurves});
  curves->setShader(lineShader);
  renderer->registerObject(curves);

  // =================================

  Shader *lightPhongTex = resourceManager->getShader(
      "light-tex", "light-phong-tex.vert", "light-phong-tex.frag");
  Object *head = resourceManager->getScene("glava/glava.obj");
  head->globalMove({-0.6, 0.25, 1.0});

  head->setShader(lightPhongTex);
  renderer->registerObject(head);

  Shader *lightPhong = resourceManager->getShader("light", "light-phong.vert",
                                                  "light-phong.frag");
  Object *plane = resourceManager->getScene("plane.obj");
  renderer->registerObject(plane);
  plane->setShader(lightPhong);
  plane->globalMove({1, 0, -3.0f});

  // Cubes
  Object *cube = resourceManager->getScene("cube/cube.obj");
  cube->setShader(lightPhongTex);
  cube->scale({0.3, 0.3, 0.3});
  InstancedObjectDecorator *instancedCube = new InstancedObjectDecorator(cube);
  instancedCube->globalMove({1.5, 0, -1.5});
  for (int i = 0; i < 20; i++) {
    instancedCube->addInstance(randomTransform());
  }
  renderer->registerObject(instancedCube);

  BSplineApproximateCurve *bspline = new BSplineApproximateCurve;
  bspline->addPoint(glm::vec3(0.f, 0.f, 0.f));
  bspline->addPoint(glm::vec3(0.f, 2.f, 1.f));
  bspline->addPoint(glm::vec3(2.f, 2.f, 2.f));
  bspline->addPoint(glm::vec3(2.f, 0.f, 3.f));
  bspline->addPoint(glm::vec3(0.f, 0.f, 4.f));
  bspline->addPoint(glm::vec3(0.f, 2.f, 5.f));
  bspline->addPoint(glm::vec3(2.f, 2.f, 6.f));
  bspline->addPoint(glm::vec3(2.f, 0.f, 7.f));
  bspline->addPoint(glm::vec3(0.f, 0.f, 8.f));
  bspline->addPoint(glm::vec3(0.f, 2.f, 9.f));
  bspline->addPoint(glm::vec3(2.f, 2.f, 10.f));
  bspline->addPoint(glm::vec3(2.f, 0.f, 11.f));

  Object *bsplineObj = new Object({bspline});
  bsplineObj->globalMove({-5, 0, -7});

  TraversableSpline *traversableSpline = new TraversableSpline(bspline);
  traversableSpline->setPoint(1e-5);

  Object *traversableObj = new Object({traversableSpline});
  traversableObj->setModelMatrix(bsplineObj->getModelMatrix());

  bsplineObj->setShader(lineShader);
  traversableObj->setShader(lineShader);
  renderer->registerObject(bsplineObj);
  renderer->registerObject(traversableObj);

  Material f16Material { {0.5f, 0.5f, 0.5f}, {0.f, 1.f, 0.f}, {0.f, 0.f, 1.f}, 21 };
  Object *f16 = resourceManager->getScene("f16.obj");
  f16->setMaterial(f16Material);
  f16->setModelMatrix(bsplineObj->getModelMatrix());

  f16->localMove(traversableSpline->Position());
  f16->setOrientation(traversableSpline->Tangent(), traversableSpline->Bitangent());

  f16->setShader(lightPhong);
  renderer->registerObject(f16);

  double totalTime = 0.;

  glEnable(GL_CULL_FACE);
  while (glfwWindowShouldClose(window) == false) {
    float deltaTime = (float)FPSManagerObject.enforceFPS(true);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    InputManager::pollEvents(deltaTime);

    renderer->update(deltaTime);
    bezierManager->update(deltaTime);

    totalTime += deltaTime;

    double t = fmod(totalTime, 5.0)/5.0;
    traversableSpline->setPoint(t);
    if (false) {
      f16->setPosition(bsplineObj->PositionVec() + traversableSpline->Position());
      f16->setOrientation(traversableSpline->Tangent(), traversableSpline->Bitangent());
    } else {
      f16->setModelMatrix(glm::mat4(1)); // reset model matrix
      glm::vec3 axis = glm::cross(f16->FrontVec(), glm::normalize(traversableSpline->Tangent()));
      glm::mat4 model(1);
      float angle = glm::acos(glm::dot(glm::normalize(traversableSpline->Tangent()), glm::normalize(f16->FrontVec())));
      model = glm::translate(model, bsplineObj->PositionVec() + traversableSpline->Position());
      model = glm::rotate(model, angle, axis);
      f16->setModelMatrix(model);
    }
    std::cout << glm::to_string(camera.FrontVec()) << std::endl;

    renderer->render();

    glfwSwapBuffers(window);
    glfwPollEvents();

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);
  }

  delete instancedCube;
  delete bezierManager;
  delete curves;
  delete bsplineObj;
  delete traversableObj;
  delete resourceManager;

  glfwTerminate();

  return EXIT_SUCCESS;
}
