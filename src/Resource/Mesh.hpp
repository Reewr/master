#pragma once

#include <memory>
#include <mmm.hpp>
#include <vector>

#include "../Log.hpp"
#include "../OpenGLHeaders.hpp"
#include "Resource.hpp"

class Texture;
class Asset;

struct aiScene;
struct aiNode;
class Mesh;
class ResourceManager;
class Program;

//! Structure to help loading meshes
class SubMesh {
public:
  SubMesh();

  SubMesh(Mesh*            model,
          ResourceManager* r,
          const aiScene*   scene,
          const aiNode*    node);

  //! Draws the submesh
  void draw(const mmm::mat4& modelMatrix, std::shared_ptr<Program> program);

private:
  int mStartIndex;
  int mSize;

  mmm::mat4 mTransform;

  std::vector<SubMesh>     mChildren;
  std::shared_ptr<Texture> mTexture;
};

/**
 * @brief Holds a mesh
 */
class Mesh : public Resource, public Logging::Log {
public:
  Mesh();
  ~Mesh();

  // Draws the mesh with the given model matrix and program
  void draw(const mmm::mat4& modelMatrix, std::shared_ptr<Program> program);

  // loads the mesh
  bool load(ResourceManager* r);
  // unloads the mesh from memory
  void unload();

  void addVertices(float x, float y, float z);
  void addTexCoords(float x, float y);
  void addNormals(float x, float y, float z);

  void setSize(int numVerts);
  int size();

  const std::vector<float>& data();

private:
  GLuint mVBO;
  GLuint mVAO;

  int                      mNumVertices;
  SubMesh*                 mMesh;
  std::vector<float>       mData;
  std::shared_ptr<Texture> texture;
};
