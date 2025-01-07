#include "MyGLM.h"

glm::mat4 MyGLM::translate3D(glm::vec3 translateVector) {
  return {
      1, 0, 0, 0,
      0, 1, 0, 0,
      0, 0, 1, 0,
      translateVector.x, translateVector.y, translateVector.z, 1
  };
}

glm::mat4 MyGLM::scale3D(glm::vec3 scaleVector) {
  return {
    scaleVector.x, 0, 0, 0,
    0, scaleVector.y, 0, 0,
    0, 0, scaleVector.z, 0,
    0, 0,             0, 1
  };
}

glm::mat4 MyGLM::rotate(glm::vec3 axis, float angle) {
  axis = glm::normalize(axis);
  // https://en.wikipedia.org/wiki/Rodrigues'_rotation_formula

  const float c = cos(glm::radians(angle));
  const float s = sin(glm::radians(angle));
  const float t = 1 - c;

  glm::mat4 mat(1);
  mat[0][0] = t * axis.x * axis.x + c;
  mat[0][1] = t * axis.x * axis.y - s * axis.z;
  mat[0][2] = t * axis.x * axis.z + s * axis.y;
  mat[1][0] = t * axis.x * axis.y + s * axis.z;
  mat[1][1] = t * axis.y * axis.y + c;
  mat[1][2] = t * axis.y * axis.z - s * axis.x;
  mat[2][0] = t * axis.x * axis.z - s * axis.y;
  mat[2][1] = t * axis.y * axis.z + s * axis.x;
  mat[2][2] = t * axis.z * axis.z + c;
  return mat;
}

glm::mat4 MyGLM::lookAtMatrix(glm::vec3 eye, glm::vec3 center, glm::vec3 viewUp) {
  const glm::vec3 front = glm::normalize(center - eye);
  const glm::vec3 right = glm::normalize(glm::cross(front, viewUp));
  const glm::vec3 up = glm::normalize(glm::cross(right, front));

  glm::mat4 mat(1);
  mat[0][0] =  right.x;
  mat[1][0] =  right.y;
  mat[2][0] =  right.z;
  mat[0][1] =  up.x;
  mat[1][1] =  up.y;
  mat[2][1] =  up.z;
  mat[0][2] = -front.x; // Ako bi front gledao naprijed, koodrinatne osi bi bile left-handed.
  mat[1][2] = -front.y; // Moramo negirati front, tako da objektov front vektor gleda
  mat[2][2] = -front.z; // u negativnu stranu Z osi.
  mat[3][0] = -glm::dot(right, eye);
  mat[3][1] = -glm::dot(up, eye);
  mat[3][2] =  glm::dot(front, eye);
  return mat;
}

glm::mat4 MyGLM::frustum(float l, float r , float b, float t , float n, float f ) {
  glm::mat4 mat(1);

  mat[0][0] = 2 * n / (r - l);
  mat[1][1] = 2 * n / (t - b);
  mat[2][0] = (r + l) / (r - l);
  mat[2][1] = (t + b) / (t - b);
  mat[2][2] = - (f + n) / (f - n);
  mat[2][3] = -1;
  mat[3][2] = -2 * f * n / (f - n);
  mat[3][3] = 0;

  return mat;
}
