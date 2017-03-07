#include "Mesh.hpp"
#include "../GLSL/Program.hpp"
#include "../Utils/Asset.hpp"
#include "ResourceManager.hpp"
#include "Texture.hpp"

#include <limits>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

using mmm::mat4;

size_t Mesh::npos = std::numeric_limits<unsigned int>::max();

Mesh::Mesh() : Logging::Log("Mesh"), mVBO(0), mVAO(0) {}

Mesh::~Mesh() {
  unload();
}

/**
 * @brief
 *   Loads the mesh by reading it from the filename that was given
 *   to it by the resource manager. The mesh is then divided into
 *   submeshes that are all stored within one object.
 *
 *   The submeshes may have a texture or more children associated
 *   with it.
 *
 *   This function will also create and initialize the Vertex buffer
 *   and Vertex arrays used for drawing.
 *
 * @param manager
 *
 * @return
 */
bool Mesh::load(ResourceManager* manager) {
  Assimp::Importer importer;
  const aiScene*   scene =
    importer.ReadFile(filename().c_str(),
                      aiProcess_Triangulate | aiProcess_OptimizeGraph |
                        aiProcess_OptimizeMeshes
                      // aiProcess_CalcTangentSpace |
                      // aiProcess_GenUVCoords |
                      // aiProcess_ValidateDataStructure |
                      // aiProcess_PreTransformVertices |
                      // aiProcess_SplitLargeMeshes |
                      // aiProcess_RemoveComponent
                      );

  if (!scene) {
    mLog->error("Unable to load mesh: {}", filename());
    return false;
  }

  addSubMesh(SubMesh(this, manager, scene, scene->mRootNode));

  mLog->debug("Loaded '{}': {} vertices", filename(), numVertices());

  for (auto& m : mSubMeshes) {
    if (m.name().size() == 0)
      mLog->warn("Mesh of {} vertices without name", m.size());
    else
      mLog->debug("SubMesh '{}' of {} vertices loaded", m.name(), m.size());
  }

  glGenBuffers(1, &mVBO);
  glGenVertexArrays(1, &mVAO);

  glBindBuffer(GL_ARRAY_BUFFER, mVBO);
  glBindVertexArray(mVAO);

  uint64_t elSize     = sizeof(Vertex);
  uint64_t offsetTex  = 3 * sizeof(float);
  uint64_t offsetNorm = 5 * sizeof(float);

  glBufferData(GL_ARRAY_BUFFER,
               numVertices() * elSize,
               &mData[0],
               GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, elSize, (void*) (0));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, elSize, (void*) offsetTex);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, elSize, (void*) offsetNorm);

  glBindVertexArray(0);
  setLoaded(true);
  return true;
}

/**
 * @brief
 *   Unloads the mesh by deleting the submeshes and the OpenGL
 *   buffers allocated.
 */
void Mesh::unload() {
  if (!loaded())
    return;

  mLog->debug("Unloading '{}'", filename());

  setLoaded(false);
  glDeleteBuffers(1, &mVBO);
  glDeleteVertexArrays(1, &mVAO);
  mData.clear();
  mSubMeshes.clear();

  mVBO = 0;
  mVAO = 0;
}

/**
* @brief
*   Adds a vertex entry to the data list in the mesh, increasing
*   its number of vertices by 1
*
* @param v
*/
void Mesh::addVertex(const Mesh::Vertex& v) {
  mData.push_back(v);
}

/**
* @brief
*   Draws the mesh and its submeshes. Requires a model matrix
*   to set the position of the submeshes correctly.
*
*   It also requires a program that has the `model` variable
*   so it can set that properly for each mesh element
*
* @param modelMatrix
* @param program
*/
// void Mesh::draw(const mmm::mat4&         modelMatrix,
//                 std::shared_ptr<Program> program) {
//   glBindVertexArray(mVAO);
//   mMesh->draw(modelMatrix, program);
//   glBindVertexArray(0);
// }

/**
* @brief
*   Adds the submesh to the submesh array
 *
 * @param mesh
 */
void Mesh::addSubMesh(const SubMesh& mesh) {
  mSubMeshes.push_back(mesh);
}

/**
 * @brief
 *   Searches through the meshes for a mesh with the given name,
 *   if one is found, a pointer to that mesh is returned.
 *
 *   If no mesh can be found, it returns a nullptr
 *
 * @param name
 *
 * @return
 */
unsigned int Mesh::findMeshByName(const std::string& name) {
  for (size_t i = 0; i < mSubMeshes.size(); ++i) {
    if (mSubMeshes[i].name() == name)
      return i;
  }

  return npos;
}

/**
 * @brief
 *   Searches through the meshes for a mesh with the given name,
 *   if one is found, a pointer to that mesh is returned.
 *
 *   If no mesh can be found, it returns a nullptr
 *
 * @param name
 *
 * @return
 */
const SubMesh& Mesh::getMeshByIndex(unsigned int index) {
  if (index >= mSubMeshes.size())
    throw new std::range_error("Index is out of bounds of submeshes");

  return mSubMeshes[index];
}


/**
 * @brief
 *   Returns the number of vertices stored in the mesh
 *   so far.
 *
 * @return
 */
unsigned int Mesh::numVertices() {
  return mData.size();
}

/**
 * @brief
 *   Returns the number of sub meshes within the mesh
 *
 * @return
 */
unsigned int Mesh::numSubMeshes() {
  return mSubMeshes.size();
}

/**
 * @brief
 *   Returns the data that is stored in the mesh. The data consists of vertices,
 *   texcoords and normals.
 *
 * @return
 */
const std::vector<Mesh::Vertex>& Mesh::data() {
  return mData;
}

/**
 * @brief
 *   Returns the names for all the meshes.
 *
 * @return
 */
std::vector<std::string> Mesh::names() {
  std::vector<std::string> names;
  names.reserve(mSubMeshes.size());

  for (auto& p : mSubMeshes)
    names.push_back(p.name());

  return names;
}

SubMesh::SubMesh() : mStartIndex(0), mSize(0), mIndex(0) {}

/**
 * @brief
 *   Creates a submesh that it extracts from
 *   the scene
 *
 * @param model
 * @param manager
 * @param scene
 * @param node
 */
SubMesh::SubMesh(Mesh*            model,
                 ResourceManager* manager,
                 const aiScene*   scene,
                 const aiNode*    node,
                 const mmm::mat4& transform)
    : mStartIndex(model->numVertices())
    , mSize(0)
    , mIndex(model->numSubMeshes()) {
  aiMatrix4x4 am = node->mTransformation;
  mTransform     = transform * mat4(am.a1,
                                am.a2,
                                am.a3,
                                am.a4,
                                am.b1,
                                am.b2,
                                am.b3,
                                am.b4,
                                am.c1,
                                am.c2,
                                am.c3,
                                am.c4,
                                am.d1,
                                am.d2,
                                am.d3,
                                am.d4);

  // Add name if it exists
  if (node->mName.length == 0)
    mName = "";
  else
    mName = std::string(node->mName.C_Str());

  // parse the node
  for (unsigned int i = 0; i < node->mNumMeshes; i += 1) {
    const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

    if (!mesh->HasPositions() || !mesh->HasTextureCoords(0) ||
        !mesh->HasNormals())
      break;

    // might break if a mesh don't have normals, texcords etc...
    for (unsigned int j = 0; j < mesh->mNumFaces; j += 1) {
      const aiFace& face = mesh->mFaces[j];

      if (face.mNumIndices < 3)
        break;

      aiVector3D* const* texCoords = mesh->mTextureCoords;
      aiVector3D*        vertices  = mesh->mVertices;
      aiVector3D*        normals   = mesh->mNormals;

      for (unsigned int fIndex = 0; fIndex < 3; ++fIndex) {
        unsigned int i1    = face.mIndices[fIndex];
        mmm::vec3 vertex   = { vertices[i1].x, vertices[i1].y, vertices[i1].z };
        mmm::vec2 texCoord = { texCoords[0][i1].x, texCoords[0][i1].y };
        mmm::vec3 normal   = { normals[i1].x, normals[i1].y, normals[i1].z };

        model->addVertex({ vertex, texCoord, normal });
      }
    }

    // load the texture for the particular mesh
    aiString texpath;
    aiReturn hasTexture =
      scene->mMaterials[mesh->mMaterialIndex]->GetTexture(aiTextureType_DIFFUSE,
                                                          0,
                                                          &texpath);
    if (hasTexture == AI_SUCCESS) {
      std::string texName = texpath.C_Str();
      mTexture            = manager->get<Texture>("Texture::" + texName);
    }
  }

  mSize = model->numVertices() - mStartIndex;

  // recursively parse childe nodes
  for (unsigned int i = 0; i < node->mNumChildren; ++i) {
    model->addSubMesh(
      SubMesh(model, manager, scene, node->mChildren[i], mTransform));
    // mChildren.push_back(SubMesh(model, manager, scene, node->mChildren[i]));
  }
}

/**
 * @brief
 *   Returns the index that is accociated with this mesh
 *
 * @return
 */
int SubMesh::index() {
  return mIndex;
}

/**
 * @brief
 *
 * @return
 */
int SubMesh::startIndex() {
  return mStartIndex;
}

/**
 * @brief Returns the size of the submesh
 *
 * @return
 */
int SubMesh::size() {
  return mSize;
}

/**
 * @brief
 *   Returns the transformation for the specifc mesh
 *
 * @return
 */
const mmm::mat4& SubMesh::transform() {
  return mTransform;
}

/**
 * @brief
 *   If the submesh had a name associated with it when it was created
 *   in a 3D editor, that name will be returned from this function.
 *
 *   If no name was given to it, this function will return an empty string
 *
 * @return
 */
const std::string& SubMesh::name() {
  return mName;
}

/**
 * @brief
 *   Draws the submesh with the given model matrix and program
 *
 * @param modelMatrix
 * @param program
 */
// void Mesh::SubMesh::draw(const mmm::mat4&         modelMatrix,
//                    std::shared_ptr<Program> program) {
//   mat4 m = modelMatrix * mTransform;

//   if (mSize != 0) {
//     mTexture->bind(1);
//     program->setUniform("model", m);
//     glDrawArrays(GL_TRIANGLES, mStartIndex, mSize);
//   }

//   for (auto& childMesh : mChildren)
//     childMesh.draw(m, program);
// }
