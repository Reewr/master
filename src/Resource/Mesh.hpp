#pragma once

#include <memory>
#include <mmm.hpp>
#include <vector>
#include <string>

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
  static size_t npos;

  struct Vertex {
    mmm::vec3 vertex;
    mmm::vec2 texCoord;
    mmm::vec3 normals;
  };

  //! Structure to help loading meshes
  class SubMesh {
  public:
    SubMesh();

    SubMesh(Mesh*            model,
            ResourceManager* r,
            const aiScene*   scene,
            const aiNode*    node,
            const mmm::mat4& transform = mmm::mat4::identity);

    int index();
    int startIndex();
    int size();

    const mmm::mat4& transform();

    const std::string& name();

  private:
    int mStartIndex;
    int mSize;
    int mIndex;
    std::string mName;

    mmm::mat4 mTransform;

    std::shared_ptr<Texture> mTexture;
  };

  Mesh();
  ~Mesh();

  // Draws the mesh with the given model matrix and program
  // void draw(const mmm::mat4& modelMatrix, std::shared_ptr<Program> program);

  // loads the mesh
  bool load(ResourceManager* r);

  // unloads the mesh from memory
  void unload();

  // Adds a vertex to the list
  void addVertex(const Vertex& v);

  // Returns the number of submeshes
  unsigned int numSubMeshes();

  // returns the number of vertices
  unsigned int numVertices();

  // Adds a submesh to the list of submeshes
  void addSubMesh(const SubMesh& mesh);

  // Returns the index to the mesh that matches the given name.
  // Returns Mesh::npos if there are no mesh with the given name
  //
  // This is significantly slower than looking up by index and may possibly
  // be an O(n) operation
  unsigned int findMeshByName(const std::string& name);

  // Returns the mesh that matches the given index. Returns nullptr
  // if the index is larger than numSubMeshes
  const SubMesh& getMeshByIndex(unsigned int index);

  // returns a reference to the data
  const std::vector<Vertex>& data();

private:
  GLuint mVBO;
  GLuint mVAO;

  std::vector<SubMesh> mSubMeshes;
  std::vector<Vertex>  mData;
};
