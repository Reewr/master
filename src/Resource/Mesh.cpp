#include "Mesh.hpp"
#include "../GLSL/Program.hpp"
#include "../Utils/Asset.hpp"
#include "ResourceManager.hpp"
#include "Texture.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

using mmm::mat4;

Mesh::Mesh()
    : Logging::Log("Mesh"), mVBO(0), mVAO(0), mNumVertices(0), mMesh(nullptr) {}
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
    importer.ReadFile(filename().c_str(), aiProcess_Triangulate
                      // aiProcess_CalcTangentSpace |
                      // aiProcess_GenUVCoords |
                      // aiProcess_OptimizeGraph |
                      // aiProcess_OptimizeMeshes
                      // aiProcess_ValidateDataStructure
                      // aiProcess_PreTransformVertices
                      // aiProcess_SplitLargeMeshes |
                      // aiProcess_RemoveComponent
                      );

  if (!scene) {
    mLog->error("Unable to load mesh: {}", filename());
    return false;
  }
  mMesh = new SubMesh(this, manager, scene, scene->mRootNode);

  mLog->debug("Loading '{}': {} vertices", filename(), size());

  glGenBuffers(1, &mVBO);
  glGenVertexArrays(1, &mVAO);

  glBindBuffer(GL_ARRAY_BUFFER, mVBO);
  glBindVertexArray(mVAO);

  uint64_t elSize     = 8 * sizeof(float);
  uint64_t offsetVert = 3 * sizeof(float);
  uint64_t offsetNorm = 5 * sizeof(float);
  GLuint   normalize  = GL_FALSE;

  glBufferData(GL_ARRAY_BUFFER, size() * elSize, &mData[0], GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, normalize, elSize, (void*) (0));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, normalize, elSize, (void*) offsetVert);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, normalize, elSize, (void*) offsetNorm);

  glBindVertexArray(0);
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

  if (mMesh != nullptr) {
    delete mMesh;
    mMesh = nullptr;
  }

  setLoaded(false);
  glDeleteBuffers(1, &mVBO);
  glDeleteVertexArrays(1, &mVAO);
  mVBO = 0;
  mVAO = 0;
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
void Mesh::draw(const mmm::mat4&         modelMatrix,
                std::shared_ptr<Program> program) {
  glBindVertexArray(mVAO);
  mMesh->draw(modelMatrix, program);
  glBindVertexArray(0);
}

/**
 * @brief
 *   Adds a verticy to the Mesh's data list
 *
 * @param x
 * @param y
 * @param z
 */
void Mesh::addVertices(float x, float y, float z) {
  mData.push_back(x);
  mData.push_back(y);
  mData.push_back(z);
}

/**
 * @brief
 *   Adds texture coordinates to the mesh's data list
 *
 * @param x
 * @param y
 */
void Mesh::addTexCoords(float x, float y) {
  mData.push_back(x);
  mData.push_back(y);
}

/**
 * @brief
 *   Adds normals to the meshes data list
 *
 * @param x
 * @param y
 * @param z
 */
void Mesh::addNormals(float x, float y, float z) {
  mData.push_back(x);
  mData.push_back(y);
  mData.push_back(z);
}

/**
 * @brief
 *   Returns the number of vertices stored in the mesh
 *   so far.
 *
 * @return
 */
int Mesh::size() {
  return mNumVertices;
}

/**
 * @brief
 *   Changes the number of vertices stored in the mesh
 *
 * @param numVerts
 */
void Mesh::setSize(int numVerts) {
  mNumVertices = numVerts;
}

const std::vector<float>& Mesh::data() {
  return mData;
}

Mesh::SubMesh::SubMesh() : mStartIndex(0), mSize(0) {}

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
Mesh::SubMesh::SubMesh(Mesh*            model,
                 ResourceManager* manager,
                 const aiScene*   scene,
                 const aiNode*    node)
    : mStartIndex(0), mSize(0) {
  aiMatrix4x4 am = node->mTransformation;
  mTransform     = mat4(am.a1,
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

  // parse the node
  for (unsigned int i = 0; i < node->mNumMeshes; i += 1) {
    const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

    if (i == 0)
      mStartIndex = model->size();

    if (!mesh->HasPositions() || !mesh->HasTextureCoords(0) ||
        !mesh->HasNormals())
      break;

    // might break if a mesh don't have normals, texcords etc...
    for (unsigned int j = 0; j < mesh->mNumFaces; j += 1) {
      const aiFace& face = mesh->mFaces[j];

      if (face.mNumIndices < 3)
        break;

      mSize += 3;
      model->setSize(model->size() + 3);

      aiVector3D* const* texCoords = mesh->mTextureCoords;
      aiVector3D*        vertices  = mesh->mVertices;
      aiVector3D*        normals   = mesh->mNormals;

      for (unsigned int fIndex = 0; fIndex < 3; ++fIndex) {
        unsigned int i1 = face.mIndices[fIndex];
        model->addVertices(vertices[i1].x, vertices[i1].y, vertices[i1].z);
        model->addTexCoords(texCoords[0][i1].x, texCoords[0][i1].y);
        model->addNormals(normals[i1].x, normals[i1].y, normals[i1].z);
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

  // recursively parse childe nodes
  for (unsigned int i = 0; i < node->mNumChildren; i += 1)
    mChildren.push_back(SubMesh(model, manager, scene, node->mChildren[i]));
}

/**
 * @brief
 *   Draws the submesh with the given model matrix and program
 *
 * @param modelMatrix
 * @param program
 */
void Mesh::SubMesh::draw(const mmm::mat4&         modelMatrix,
                   std::shared_ptr<Program> program) {
  mat4 m = modelMatrix * mTransform;

  if (mSize != 0) {
    mTexture->bind(1);
    program->setUniform("model", m);
    glDrawArrays(GL_TRIANGLES, mStartIndex, mSize);
  }

  for (auto childMesh : mChildren)
    childMesh.draw(m, program);
}
