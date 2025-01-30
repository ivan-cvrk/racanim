#include "Renderer.h"
#include "Object.h"

#include "Shader.h"
#include <glm/gtc/matrix_transform.hpp>

Renderer::Renderer(Camera &camera, Light &light)
    : light(light), camera(camera) {
  glGenTextures(1, &depthMapTEX);
  glBindTexture(GL_TEXTURE_2D, depthMapTEX);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH,
               SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

  glGenFramebuffers(1, &depthMapFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                         depthMapTEX, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::render() {
  glm::mat4 projectionMatrix = this->camera.getProjectionMatrix();
  glm::mat4 viewMatrix = this->camera.getViewMatrix();

  float zNear = 1.0f, zFar = 20.0f;
  const float fH = tan(light.getOuterCutoff()) * zNear;
  const float fW = fH;
  glm::mat4 lightProjection = MyGLM::frustum(-fW, fW, -fH, fH, zNear, zFar);
  glm::mat4 lightView = light.getViewMatrix();
  glm::mat4 lightSpace = lightProjection * lightView;

  glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  glClear(GL_DEPTH_BUFFER_BIT);

  for (ISceneObject *object : objects) {
    Shader *shader = object->getShader();
    Shader* depthShader = object->getDepthShader();
    if (depthShader == nullptr) {
      continue;
    }
    object->setShader(depthShader);
    depthShader->use();
    glUniformMatrix4fv(glGetUniformLocation(depthShader->ID, "LightSpaceMatrix"),
                    1, GL_FALSE, &lightSpace[0][0]);
    object->render(projectionMatrix, viewMatrix);
    object->setShader(shader);
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, camera.getWidth(), camera.getHeight());
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, depthMapTEX);

  for (ISceneObject *object : objects) {
    Shader *shader = object->getShader();
    if (shader != nullptr) {
      shader->use();
      light.initializeUniforms(shader->ID);
      const glm::vec3 &camPosition = camera.PositionVec();
      glUniform3f(glGetUniformLocation(shader->ID, "CameraPosition"),
                  camPosition.x, camPosition.y, camPosition.z);
      glUniform1i(glGetUniformLocation(shader->ID, "depth_texture"), 0);
      glUniformMatrix4fv(glGetUniformLocation(shader->ID, "LightSpaceMatrix"),
                      1, GL_FALSE, &lightSpace[0][0]);
    }
    object->render(projectionMatrix, viewMatrix);
  }
}

void Renderer::update(float deltaTime) {
  for (auto sceneObject : this->objects) {
    sceneObject->update(deltaTime);
  }
}
