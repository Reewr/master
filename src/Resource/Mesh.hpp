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

  // sets the amount of vertices
  void setSize(int numVerts);

  // returns the amount of vertices
  int size();

  // returns a reference to the data
  const std::vector<float>& data();

  // transforms the mesh and all its submeshes
  void transform(const mmm::mat4& transform);

private:

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

    void transform(const mmm::mat4& transform);

  private:
    int mStartIndex;
    int mSize;

    mmm::mat4 mTransform;

    std::vector<SubMesh>     mChildren;
    std::shared_ptr<Texture> mTexture;
  };

  GLuint mVBO;
  GLuint mVAO;

  int                      mNumVertices;
  SubMesh*                 mMesh;
  std::vector<float>       mData;
};
