#pragma once


#include "Shader.h"
#include "IRenderable.h"
#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <unordered_map>
#include <string>
#include <vector>

// Vertex of and animated model
struct aVertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
    glm::vec4 boneIds = glm::vec4(0);
    glm::vec4 boneWeights = glm::vec4(0.0f);
};

// Structure to hold bone tree
struct Bone
{
    int id = 0; // position of the bone in final upload tree
    std::string name = "";
    glm::mat4 offset = glm::mat4(1.0f);
    std::vector<Bone> children = {};
};

// Structure representing an animation track
struct BoneTransformTrack
{
    std::vector<float> positionTimeStamps = {};
    std::vector<float> rotationTimeStamps = {};
    std::vector<float> scaleTimeStamps    = {};

    std::vector<glm::vec3> positions = {};
    std::vector<glm::quat> rotations = {};
    std::vector<glm::vec3> scales    = {};

};

// structure containing animation information
struct Animation
{
    float duration = 0.0f;
    float ticksPerSecond = 1.0f;
    std::unordered_map<std::string, BoneTransformTrack> boneTransforms = {};
};

// Animated model loads one animated mesh
class AnimatedModel : public IRenderable
{
    double totalElapsedTime = 0;
    // directory from where is model loaded
    std::string directory;

    // model data
    std::vector<aVertex> vertices = {};
    std::vector<unsigned int> indices = {};
    // vertex array objest
    unsigned int VAO = 0;

    // bone count
    unsigned int nBoneCount = 0;
    // full skeleton, bone hierarchy
    Bone skeleton;
    // animation currently supports only one
    Animation animation;
    // current pose of all bones
    std::vector<glm::mat4> currentPose = {};
    // global inverse transform matrix
    glm::mat4 globalInverseTransform;

    // load model data into class variables
    void loadModel(const aiScene* scene, aiMesh* mesh);
    // Create bone hierarchy
    bool readSkeleton(Bone& boneOutput, aiNode* node, std::unordered_map<std::string, std::pair<int, glm::mat4>>& boneInfoTable);
    // Load animation (currently supports only one animation)
    void loadAnimation(const aiScene* scene);
    // Create VAO for the model
    unsigned int createVertexArray();
    // Update pose
    void getPose(Bone& bone, const glm::mat4& parentTransform, float dt);
    // Get interpolated time
    std::pair<unsigned int, float> getTimeFraction(const std::vector<float>& times, float dt);

public:

    // can't make object without model
    AnimatedModel() = delete;

    // Load model from file
    AnimatedModel(const std::string& path);


    virtual void draw(const Shader* shader);
    virtual void update([[maybe_unused]] float deltaTime);
};
