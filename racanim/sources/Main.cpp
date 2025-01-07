#include <GLFW/glfw3.h>
#include <cstdio>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
//#include <random>

#include "BezierCurve.h"
#include "BezierManagerDecorator.h"
#include "FPSManager.h"
#include "InputManager.h"
#include "InstancedObjectDecorator.h"
#include "Object.h"
#include "PhysicsEngine.h"
#include "PhysicsInputDecorator.h"
#include "PhysicsObject.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "Shader.h"

using namespace std;

int width = 1200, height = 1000;

void framebuffer_size_callback([[maybe_unused]] GLFWwindow *window, int Width,
                               int Height) {
  width = Width;
  height = Height;
}

// Transform randomTransform() {
//   static std::random_device generator;
//   static std::normal_distribution<float> normal(0, 1.5);
//   static std::uniform_real_distribution<float> uniform(0, 90);
//
//   Transform transform;
//   transform.globalMove({normal(generator),
//                         abs(normal(generator)) / 1.5f,
//                         normal(generator) * 1.5});
//   transform.rotateFPS(uniform(generator), uniform(generator), false);
//   return transform;
// }

int main([[maybe_unused]] int argc, char *argv[]) {
  GLFWwindow *window;

  glfwInit();
  gladLoadGL();

  glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
  window = glfwCreateWindow(width, height, "racanim", nullptr, nullptr);

  if (window == nullptr) {
    fprintf(stderr, "Failed to Create OpenGL Context");
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(window);

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
  camera.setPosition({0, 1, 4});
  camera.setOrientation(-camera.FrontVec() + glm::vec3{0.15, -0.2, 0.0},
                        camera.UpVec());

  // ======== Curves ============
  BezierCurve *bezierCurve = new BezierCurve;
  // BezierManagerDecorator *bezierManager =
  //     new BezierManagerDecorator(camera, *bezierCurves);
  // InputManager::registerMoveable(bezierManager);

  Light light({0.8f, 0.8f, 0.8f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, 0.f, 0.f);
  light.globalMove({-2, 3, 3});
  light.setOrientation(-light.PositionVec(), glm::vec3{0, 1, 0});

  ResourceManager *resourceManager = new ResourceManager(argv[0]);

  Shader *depthShader =
      resourceManager->getShader("depth", "depth.vert", "depth.frag");
  Renderer *renderer = new Renderer(camera, light, depthShader);

  Shader *lineShader =
      resourceManager->getShader("line", "wire.vert", "wire.frag");
  Object *curves = new Object({bezierCurve});
  curves->setShader(lineShader);

  renderer->registerObject(curves);
  // =================================

  Shader *lightPhongTex = resourceManager->getShader(
      "light-tex", "light-phong-tex.vert", "light-phong-tex.frag");
  Object *head = resourceManager->getScene("glava/glava.obj");
  head->globalMove({-0.6, -5, 1.0});

  head->setShader(lightPhongTex);
  renderer->registerObject(head);

  Shader *lightPhong = resourceManager->getShader("light", "light-phong.vert",
                                                  "light-phong.frag");
  Object *plane = resourceManager->getScene("plane.obj");
  renderer->registerObject(plane);
  plane->setShader(lightPhong);
  plane->globalMove({1, 0, -3.0f});


  // Cubes
  // Object *cube = resourceManager->getScene("cube/cube.obj");
  // cube->setShader(lightPhongTex);
  // cube->scale({0.3, 0.3, 0.3});
  // InstancedObjectDecorator *instancedCube = new
  // InstancedObjectDecorator(cube); instancedCube->globalMove({1.5, 0, -1.5});
  // for (int i = 0; i < 30; i++) {
  //  instancedCube->addInstance(randomTransform());
  //}
  // renderer->registerObject(instancedCube);

  PhysicsEngine physicsEngine;
  PhysicsInputDecorator *inputDecorator =
      new PhysicsInputDecorator(camera, physicsEngine);
  InputManager::registerMoveable(inputDecorator);

  Object *sphere = resourceManager->getScene("sphere.obj");
  sphere->setShader(lightPhong);

  Ball ball(sphere, physicsEngine);
  renderer->registerObject(&ball);

  ClothMesh* clothMesh = new ClothMesh(physicsEngine, false);
  Shader *clothShader = resourceManager->getShader(
      "cloth", "cloth.vert", "cloth.frag", "cloth.geom");

  Object *cloth = new Object({clothMesh});
  cloth->setShader(clothShader);
  renderer->registerObject(cloth);

  Shader *particleShader = resourceManager->getShader(
      "particle", "particles.vert", "particles.frag");
  ClothMesh* clothPoints = new ClothMesh(physicsEngine, true);
  Object *clothPointsObj = new Object({clothPoints});
  clothPointsObj->setShader(particleShader);
  renderer->registerObject(clothPointsObj);

  //glEnable(GL_CULL_FACE);
  while (glfwWindowShouldClose(window) == false) {
    float deltaTime = (float)FPSManagerObject.enforceFPS(true);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    InputManager::pollEvents(deltaTime);
    physicsEngine.update(deltaTime);

    renderer->update(deltaTime);
    // bezierManager->update(deltaTime);

    renderer->render();

    glfwSwapBuffers(window);
    glfwPollEvents();

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);
  }

  // delete instancedCube;
  // delete bezierManager;
  delete curves;
  delete resourceManager;

  glfwTerminate();

  return EXIT_SUCCESS;
}
