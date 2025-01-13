#include <GLFW/glfw3.h>
#include <cstdio>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include "AnimatedModel.h"
#include "FPSManager.h"
#include "InputManager.h"
#include "Object.h"
#include "PhysicsInputDecorator.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "Shader.h"

#include <filesystem>
namespace fs = std::filesystem;

using namespace std;

int width = 1200, height = 1000;

void framebuffer_size_callback([[maybe_unused]] GLFWwindow *window, int Width,
                               int Height) {
  width = Width;
  height = Height;
}

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

  Light light({0.4f, 0.4f, 0.4f}, {0.8f, 0.8f, 0.8f}, {1.0f, 1.0f, 1.0f}, 25.f, 26.f);
  light.globalMove({-1, 1.3, 1});
  light.setOrientation(-light.PositionVec()+glm::vec3{0, 0.3, 0}, glm::vec3{0, 1, 0});

  ResourceManager *resourceManager = new ResourceManager(argv[0]);

  Shader *depthShader =
      resourceManager->getShader("depth", "depth.vert", "depth.frag");
  Renderer *renderer = new Renderer(camera, light);



  Shader *lightPhong = resourceManager->getShader("light", "light-phong.vert",
                                                  "light-phong.frag");
  Object *plane = resourceManager->getScene("plane.obj");
  renderer->registerObject(plane);
  plane->setShader(lightPhong);
  plane->setDepthShader(depthShader);
  //.plane->scale({0.2, 1, 0.2});
  //plane->scale({1, 1, 0.2});

  InputManager::registerMoveable(&camera);

  Shader *animatedShader =
      resourceManager->getShader("animated", "animated.vert", "animated.frag");

  Shader *animatedDepthShader =
      resourceManager->getShader("animated-depth", "animated-depth.vert", "animated-depth.frag");

  fs::path executable_path = fs::canonical(fs::path(argv[0]));
  fs::path directory = executable_path.parent_path() / "resources";
  fs::path resource = directory / "man/model.dae";

  AnimatedModel* animatedModel = new AnimatedModel(resource.string());
  Object *animatedModelObj = new Object({animatedModel});
  animatedModelObj->setMaterial(resourceManager->getMaterial(resource.c_str(), 0));
  animatedModelObj->setShader(animatedShader);
  renderer->registerObject(animatedModelObj);
  animatedModelObj->setOrientation({0.f, 1.f, 0.f}, {0.f, 0.f, 1.f});
  animatedModelObj->rotateFPS(90, 0, 0);
  animatedModelObj->scale({0.1f, 0.1f, 0.1f});
  animatedModelObj->setDepthShader(animatedDepthShader);

  glEnable(GL_CULL_FACE);
  while (glfwWindowShouldClose(window) == false) {
    float deltaTime = (float)FPSManagerObject.enforceFPS(true);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    InputManager::pollEvents(deltaTime);
    //physicsEngine.update(deltaTime);

    renderer->update(deltaTime);
    // bezierManager->update(deltaTime);

    renderer->render();

    glfwSwapBuffers(window);
    glfwPollEvents();

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);
  }

  delete animatedModel;
  delete resourceManager;

  glfwTerminate();

  return EXIT_SUCCESS;
}
