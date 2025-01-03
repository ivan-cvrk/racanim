#include "Shader.h"
#include <iostream>
#include <fstream>
#include <sstream>

extern const char *argv0;

void Shader::checkCompilerErrors(unsigned int shader, std::string type) {
  int success;
  char infolog[1024];
  if (type != "PROGRAM") {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
      glGetShaderInfoLog(shader, 1024, nullptr, infolog);
      fprintf(stderr,
              "ERROR::SHADER_COMPILATION_ERROR of type: "
              "%s\n%s\n-----------------------------------------------------\n",
              type.c_str(), infolog);
    }
  } else {
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(shader, 1024, nullptr, infolog);
      fprintf(
          stderr,
          "ERROR::PROGRAM_LINKING_ERROR of type: "
          "%s\n%s\n-------------------------------------------------------\n",
          type.c_str(), infolog);
    }
  }
}

GLuint Shader::loadVertexShader(const char *path) {
  std::string vertexCode;
  std::ifstream vShaderFile;

  vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    // open files
    vShaderFile.open(path);
    std::stringstream vShaderStream;
    // read file's buffer contents into streams
    vShaderStream << vShaderFile.rdbuf();
    // close file handlers
    vShaderFile.close();
    // convert stream into string
    vertexCode = vShaderStream.str();

  } catch (std::ifstream::failure& e) {
    fprintf(stderr, "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ\n");
  }

  const char *vShaderCode = vertexCode.c_str();

  GLint vertex;
  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vShaderCode, NULL);
  glCompileShader(vertex);
  checkCompilerErrors(vertex, "VERTEX");

  return vertex;
}

GLuint Shader::loadFragmentShader(const char *path) {
  std::string fragmentCode;
  std::ifstream fShaderFile;
  // ensure ifstream objects can throw exceptions:
  fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    // open files
    fShaderFile.open(path);
    std::stringstream fShaderStream;
    // read file's buffer contents into streams
    fShaderStream << fShaderFile.rdbuf();
    // close file handlers
    fShaderFile.close();
    // convert stream into string
    fragmentCode = fShaderStream.str();

  } catch (std::ifstream::failure& e) {
    fprintf(stderr, "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ\n");
  }

  const char *fShaderCode = fragmentCode.c_str();

  GLuint fragment;
  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fShaderCode, NULL);
  glCompileShader(fragment);
  checkCompilerErrors(fragment, "FRAGMENT");
  return fragment;
}


GLuint Shader::loadGeometryShader(const char *path) {
  std::string geometryCode;
  std::ifstream gShaderFile;
  // ensure ifstream objects can throw exceptions:
  gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    // open files
    gShaderFile.open(path);
    std::stringstream gShaderStream;
    // read file's buffer contents into streams
    gShaderStream << gShaderFile.rdbuf();
    // close file handlers
    gShaderFile.close();
    // convert stream into string
    geometryCode = gShaderStream.str();

  } catch (std::ifstream::failure& e) {
    fprintf(stderr, "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ\n");
  }

  const char *gShaderCode = geometryCode.c_str();
  GLuint geometry;
  // fragment Shader
  geometry = glCreateShader(GL_GEOMETRY_SHADER);
  glShaderSource(geometry, 1, &gShaderCode, NULL);
  glCompileShader(geometry);
  checkCompilerErrors(geometry, "GEOMETRY");
  return geometry;
}



Shader::Shader(const char *vertexPath, const char *fragmentPath, const char *geometryPath = nullptr) {
  GLuint vertex = loadVertexShader(vertexPath);
  GLuint fragment = loadFragmentShader(fragmentPath);

  // shader Program
  ID = glCreateProgram();
  glAttachShader(ID, vertex);
  glAttachShader(ID, fragment);

  GLuint geometry;
  if (geometryPath != nullptr) {
    geometry = loadGeometryShader(geometryPath);
    glAttachShader(ID, geometry);
  }

  glLinkProgram(ID);
  checkCompilerErrors(ID, "PROGRAM");

  glDeleteShader(vertex);
  glDeleteShader(fragment);

  if (geometryPath != nullptr) {
    glDeleteShader(geometry);
  }
}

Shader::~Shader() { glDeleteProgram(ID); }

void Shader::use() { glUseProgram(ID); }

void Shader::setUniform(const std::string &name, bool value) const {
  glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setUniform(const std::string &name, int value) const {
  glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setUniform(const std::string &name, float value) const {
  glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

