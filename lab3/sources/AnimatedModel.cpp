#include "AnimatedModel.h"
#include "glm/gtc/type_ptr.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/io.hpp>
#include <iostream>

// Generate glm::mat3 out of assimp aiMatrix4x4
inline glm::mat4 AssimpToGlmMatrix(aiMatrix4x4 mat) {
  glm::mat4 m;
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      m[x][y] = mat[y][x];
    }
  }
  return m;
}

// Generate glm::vec3 out of assimp aiVector3D
inline glm::vec3 AssimpToGlmVec3(aiVector3D vec) {
  return glm::vec3(vec.x, vec.y, vec.z);
}

// Generate glm::quat out od assimp aiQuaternion
inline glm::quat AssimpToGlmQuat(aiQuaternion quat) {
  glm::quat q;
  q.x = quat.x;
  q.y = quat.y;
  q.z = quat.z;
  q.w = quat.w;

  return q;
}

// constructof of the class loading model from a file
AnimatedModel::AnimatedModel(const std::string& path) {
  Assimp::Importer importer;

  const aiScene *scene =
      importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs |
                                  aiProcess_GenSmoothNormals);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
  }

  directory = path.substr(0, path.find_last_of('/'));

  // Load only first mesh from scene
  aiMesh *mesh = scene->mMeshes[0];

  scene->mRootNode->mTransformation = aiMatrix4x4();
  globalInverseTransform = AssimpToGlmMatrix(scene->mRootNode->mTransformation);
  globalInverseTransform = glm::inverse(globalInverseTransform);

  loadModel(scene, mesh);
  loadAnimation(scene);

  VAO = createVertexArray();

  glm::mat4 identity = glm::mat4(1.0f);
  currentPose.resize(nBoneCount, identity);
}

// Load model vertice data
void AnimatedModel::loadModel(const aiScene *scene, aiMesh *mesh) {
  // Load position, normal, uv
  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    aVertex vertex;
    glm::vec3 vector;

    // Process position
    vector.x = mesh->mVertices[i].x;
    vector.y = mesh->mVertices[i].y;
    vector.z = mesh->mVertices[i].z;
    vertex.position = vector;

    // Process normals
    if (mesh->HasNormals()) {
      vector.x = mesh->mNormals[i].x;
      vector.y = mesh->mNormals[i].y;
      vector.z = mesh->mNormals[i].z;
      vertex.normal = vector;
    }
    // Process uv
    if (mesh->mTextureCoords[0]) {
      glm::vec2 vec;
      vec.x = mesh->mTextureCoords[0][i].x;
      vec.y = mesh->mTextureCoords[0][i].y;
      vertex.uv = vec;
    } else {
      vertex.uv = glm::vec2(0.0f);
    }

    vertex.boneIds = glm::ivec4(0);
    vertex.boneWeights = glm::vec4(0.0f);

    vertices.push_back(vertex);
  }

  // Load bone data to vertices
  std::unordered_map<std::string, std::pair<int, glm::mat4>> boneInfo;
  std::vector<unsigned int> boneCounts;
  boneCounts.resize(vertices.size(), 0);
  nBoneCount = mesh->mNumBones;

  // Loop trough each bone
  for (unsigned int i = 0; i < nBoneCount; i++) {
    aiBone *bone = mesh->mBones[i];
    glm::mat4 m = AssimpToGlmMatrix(bone->mOffsetMatrix);
    boneInfo[bone->mName.C_Str()] = {i, m};

    // Loop trough each vertex that has this bone
    for (unsigned int j = 0; j < bone->mNumWeights; j++) {
      unsigned int id = bone->mWeights[j].mVertexId;
      float weight = bone->mWeights[j].mWeight;
      boneCounts[id]++;

      switch (boneCounts[id]) {
      case 1:
        vertices[id].boneIds.x = i;
        vertices[id].boneWeights.x = weight;
        break;
      case 2:
        vertices[id].boneIds.y = i;
        vertices[id].boneWeights.y = weight;
        break;
      case 3:
        vertices[id].boneIds.z = i;
        vertices[id].boneWeights.z = weight;
        break;
      case 4:
        vertices[id].boneIds.w = i;
        vertices[id].boneWeights.w = weight;
      }
    }
  }

  // Normalize weights to make all weights sum to 1
  for (unsigned int i = 0; i < vertices.size(); i++) {
    glm::vec4 &boneWeights = vertices[i].boneWeights;
    float totalWeight =
        boneWeights.x + boneWeights.y + boneWeights.z + boneWeights.w;
    if (totalWeight > 0.0f) {
      vertices[i].boneWeights =
          glm::vec4(boneWeights.x / totalWeight, boneWeights.y / totalWeight,
                    boneWeights.z / totalWeight, boneWeights.w / totalWeight);
    }
  }

  // Load indices
  for (unsigned i = 0; i < mesh->mNumFaces; i++) {
    aiFace &face = mesh->mFaces[i];
    for (unsigned int j = 0; j < face.mNumIndices; j++)
      indices.push_back(face.mIndices[j]);
  }

  // Create bone hierarchy
  readSkeleton(skeleton, scene->mRootNode, boneInfo);
}

// Make bone hierarchy
bool AnimatedModel::readSkeleton(
    Bone &boneOutput, aiNode *node,
    std::unordered_map<std::string, std::pair<int, glm::mat4>> &boneInfoTable) {
  if (boneInfoTable.find(node->mName.C_Str()) != boneInfoTable.end()) {
    boneOutput.name = node->mName.C_Str();
    boneOutput.id = boneInfoTable[boneOutput.name].first;
    boneOutput.offset = boneInfoTable[boneOutput.name].second;

    for (unsigned i = 0; i < node->mNumChildren; i++) {
      Bone child;
      readSkeleton(child, node->mChildren[i], boneInfoTable);
      boneOutput.children.push_back(child);
    }
    return true;
  } else {
    for (unsigned int i = 0; i < node->mNumChildren; i++)
      if (readSkeleton(boneOutput, node->mChildren[i], boneInfoTable))
        return true;
  }
  return false;
}

// Load one animation
void AnimatedModel::loadAnimation(const aiScene *scene) {
  // Load only first animation
  aiAnimation *anim = scene->mAnimations[0];

  if (anim->mTicksPerSecond != 0.0f)
    animation.ticksPerSecond = anim->mTicksPerSecond;
  else
    animation.ticksPerSecond = 1;

  animation.duration = anim->mDuration * 1.0 / anim->mTicksPerSecond;
  animation.boneTransforms = {};

  // load positions rotations and scales for each bone
  // each channel represents each bone
  for (unsigned int i = 0; i < anim->mNumChannels; i++) {
    aiNodeAnim *channel = anim->mChannels[i];
    BoneTransformTrack track;

    for (unsigned int j = 0; j < channel->mNumPositionKeys; j++) {
      track.positionTimeStamps.push_back(channel->mPositionKeys[j].mTime / animation.ticksPerSecond);
      track.positions.push_back(
          AssimpToGlmVec3(channel->mPositionKeys[j].mValue));
    }
    for (unsigned int j = 0; j < channel->mNumRotationKeys; j++) {
      track.rotationTimeStamps.push_back(channel->mRotationKeys[j].mTime / animation.ticksPerSecond);
      track.rotations.push_back(
          AssimpToGlmQuat(channel->mRotationKeys[j].mValue));
    }
    for (unsigned int j = 0; j < channel->mNumScalingKeys; j++) {
      track.scaleTimeStamps.push_back(channel->mScalingKeys[j].mTime / animation.ticksPerSecond);
      track.scales.push_back(AssimpToGlmVec3(channel->mScalingKeys[j].mValue));
    }
    animation.boneTransforms[channel->mNodeName.C_Str()] = track;
  }
}

// Create Vertex Array object used for drawing the model
unsigned int AnimatedModel::createVertexArray() {
  unsigned int VAO = 0, VBO = 0, EBO = 0;

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  // vertices
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(aVertex) * vertices.size(), &vertices[0],
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(aVertex),
                        (void *)offsetof(aVertex, position));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(aVertex),
                        (void *)offsetof(aVertex, normal));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(aVertex),
                        (void *)offsetof(aVertex, uv));
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(aVertex),
                        (void *)offsetof(aVertex, boneIds));
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(aVertex),
                        (void *)offsetof(aVertex, boneWeights));

  // indices
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               &indices[0], GL_STATIC_DRAW);
  glBindVertexArray(0);

  return VAO;
}

//==================DRAWING==================================================
// Functions defined below are used for playing animations and drawing model.

// get time stamp and its interpolated fraction
std::pair<unsigned int, float>
AnimatedModel::getTimeFraction(const std::vector<float> &times, float dt) {
  unsigned int segment = 0;
  while (dt > times[segment]) {
    segment++;
  }
  float start = times[segment - 1];
  float end = times[segment];
  float frac = (dt - start) / (end - start);
  return {segment, frac};
}

// Possible optimizations with resuing some matrices
void AnimatedModel::getPose(Bone &bone, const glm::mat4 &parentTransform,
                            float dt) {
  BoneTransformTrack &btt = animation.boneTransforms[bone.name];
  // reminder of dividing two floats
  dt = fmod(dt, animation.duration);

  std::pair<unsigned int, float> fp;

  // Get frame to which you are interpolating and interpolation fraction
  fp = getTimeFraction(btt.positionTimeStamps, dt);

  // Calculate inerpolated position between two key frames
  glm::vec3 position1 = btt.positions[fp.first - 1];
  glm::vec3 position2 = btt.positions[fp.first];

  glm::vec3 position = glm::mix(position1, position2, fp.second);

  // Calculate inerpolated rotation between two key frames
  glm::quat rotation1 = btt.rotations[fp.first - 1];
  glm::quat rotation2 = btt.rotations[fp.first];

  glm::quat rotation = glm::slerp(rotation1, rotation2, fp.second);

  // Calculate interpolated scale between two key frames
  glm::vec3 scale1 = btt.scales[fp.first - 1];
  glm::vec3 scale2 = btt.scales[fp.first];

  glm::vec3 scale = glm::mix(scale1, scale2, fp.second);

  // Calculate local transform
  glm::mat4 positionMat = glm::mat4(1.0f);
  positionMat = glm::translate(positionMat, position);

  glm::mat4 rotationMat = glm::toMat4(rotation);

  glm::mat4 scaleMat = glm::mat4(1.0f);
  scaleMat = glm::scale(scaleMat, scale);

  glm::mat4 localTransform = positionMat * rotationMat * scaleMat;
  glm::mat4 globalTransform = parentTransform * localTransform;

  currentPose[bone.id] = globalInverseTransform * globalTransform * bone.offset;

  for (Bone &child : bone.children) {
    getPose(child, globalTransform, dt);
  }
}

void AnimatedModel::update(float elapsedTime) {
  totalElapsedTime += elapsedTime;
  getPose(skeleton, glm::mat4(1.0f), totalElapsedTime);
}

// Make model draw itself, provide it with elapsed time for animation drawing
void AnimatedModel::draw(const Shader* shader) {
  unsigned int boneMatricesLocation =
      glGetUniformLocation(shader->ID, "bone_transforms");

  glUniformMatrix4fv(boneMatricesLocation, nBoneCount, GL_FALSE,
                     glm::value_ptr(currentPose[0]));
  glBindVertexArray(VAO);

  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
  glActiveTexture(GL_TEXTURE0);
}
