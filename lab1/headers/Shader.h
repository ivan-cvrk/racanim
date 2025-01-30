#pragma once

#include <glad/glad.h>

#include <string>

class Shader {
private:
  void checkCompilerErrors(unsigned int shader, std::string type);
  GLuint loadVertexShader(const char *path);
  GLuint loadFragmentShader(const char *path);
  GLuint loadGeometryShader(const char *path);
public:
  unsigned int ID;

  Shader(const char *vertexPath, const char *fragmentPath, const char *geometryPath);

  ~Shader();

  void use();
  void setUniform(const std::string &name, bool value) const;
  void setUniform(const std::string &name, int value) const;
  void setUniform(const std::string &name, float value) const;

  static Shader *loadShader(const char *naziv);
};
