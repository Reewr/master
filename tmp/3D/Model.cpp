#include <Graphical/Framebuffer.hpp>
#include <3D/Model.hpp>
#include <Utils/Utils.hpp>
#include <Utils/CFG.hpp>

#include <GLFW/glfw3.h>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


Program Model::program;

template <typename ModelT>
Mesh::Mesh (ModelT& model, const aiScene* scene, const aiNode* node) {
  aiMatrix4x4 am = node->mTransformation;
  transform = {
    am.a1, am.a2, am.a3, am.a4,
    am.b1, am.b2, am.b3, am.b4,
    am.c1, am.c2, am.c3, am.c4,
    am.d1, am.d2, am.d3, am.d4
  };

  // parse the node
  for (unsigned int i = 0; i < node->mNumMeshes; i += 1) {
    const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

    if (i == 0)
      startIndex = model.vertices;

    // might break if a mesh don't have normals, texcords etc...
    for (unsigned int j = 0; j < mesh->mNumFaces; j += 1) {
      const aiFace& face = mesh->mFaces[j];

      for (int k = 0; k < 3; k += 1) {
        if (face.mNumIndices < 3) break;
        if ( mesh->HasPositions() &&
             mesh->HasTextureCoords(0) &&
             mesh->HasNormals()) {
  
          size += 1; //mesh->mNumVertices;
          model.vertices += 1;
  
          model.data.push_back(mesh->mVertices[face.mIndices[k]].x);
          model.data.push_back(mesh->mVertices[face.mIndices[k]].y);
          model.data.push_back(mesh->mVertices[face.mIndices[k]].z);

          model.data.push_back(mesh->mTextureCoords[0][face.mIndices[k]].x);
          model.data.push_back(mesh->mTextureCoords[0][face.mIndices[k]].y);

          model.data.push_back(mesh->mNormals[face.mIndices[k]].x);
          model.data.push_back(mesh->mNormals[face.mIndices[k]].y);
          model.data.push_back(mesh->mNormals[face.mIndices[k]].z);
        }
      }
    }

    // load the texture for the particular mesh
    aiString texpath;
    aiReturn hasTexture = scene->mMaterials[mesh->mMaterialIndex]->
                          GetTexture(aiTextureType_DIFFUSE, 0, &texpath);
    if (hasTexture == AI_SUCCESS) {
      std::string s = texpath.C_Str();
      s = "../media/models/" + s;
      texture = Texture(s, Texture::TEXTURE);
    }
  }

  // recursively parse childe nodes
  for (unsigned int i = 0; i < node->mNumChildren; i += 1)
    children.push_back(Mesh(model, scene, node->mChildren[i]));
}
// explicit instantiations
template Mesh::Mesh (Model& model, const aiScene* scene, const aiNode* node);
template Mesh::Mesh (DeferredModel& model, const aiScene* scene, const aiNode* node);

void Mesh::draw (const mat4& ModelMatrix) {
  mat4 m = ModelMatrix*transform;

  if (size != 0) {
    texture.bind(1);
    Model::program.setUniform("model", m);
    glDrawArrays(GL_TRIANGLES, startIndex, size);
  }

  for (auto c: children)
    c.draw(m);
}

void Mesh::draw (const mat4& ModelMatrix, Framebuffer* shadowmap) {
  mat4 m = ModelMatrix*transform;

  if (size != 0) {
    shadowmap->program->setUniform ("model", m);
    glDrawArrays (GL_TRIANGLES, startIndex, size);
  }

  for (auto c: children)
    c.draw (m, shadowmap);
}


void Model::init (CFG* cfg) {
  program.createProgram ("shaders/Model.fs", "shaders/Model.vs", false);
  program.bindAttribs(
    {"vertex_position", "vertex_texCoord", "vertex_normal"},
    {0,1,2}
  );
  program.link();

  program.setUniform("shadowMap", 0, "diffuseMap", 1);
  program.setUniform("shadowSamples", cfg->graphics.shadowSamples);
}

void Model::deinit() {
  program.deleteProgram();
}

Model::Model (const char* filePath) {
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(filePath, 
    aiProcess_Triangulate
    //aiProcess_CalcTangentSpace |
    //aiProcess_GenUVCoords |
    //aiProcess_OptimizeGraph |
    //aiProcess_OptimizeMeshes
    //aiProcess_ValidateDataStructure 
    //aiProcess_PreTransformVertices 
    //aiProcess_SplitLargeMeshes |
    //aiProcess_RemoveComponent 
  );

  if (!scene) {
    error("failed to open ", filePath);
    return;
  }

  root = Mesh(*this, scene, scene->mRootNode);

  log ("loading: model ", filePath, " ", vertices, " vertices");

  glGenBuffers (1, &vbo);
  glBindBuffer (GL_ARRAY_BUFFER, vbo);
  glBufferData (GL_ARRAY_BUFFER, vertices * 8 * sizeof (float), &data[0], GL_STATIC_DRAW);

  glGenVertexArrays (1, &vao);
  glBindVertexArray (vao);
  glBindBuffer (GL_ARRAY_BUFFER, vbo);

  glEnableVertexAttribArray (0);
  glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(0));
  glEnableVertexAttribArray (1);
  glVertexAttribPointer (1, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
  glEnableVertexAttribArray (2);
  glVertexAttribPointer (2, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(5*sizeof(float)));

  /* TBI, remember to set aray sizei to 4 and size to 12 * sizeof...
  glEnableVertexAttribArray (3);
  glVertexAttribPointer (3, 4, GL_FLOAT, GL_FALSE, 12*sizeof(float), (void*)(8*sizeof(float)));
  */

  glBindVertexArray(0);
  data.clear();
}

Model::~Model () {
  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);
}

void Model::draw (const mat4& ModelMatrix) {
  glBindVertexArray (vao);
  program.bind ();
  root.draw (ModelMatrix);

  glBindVertexArray (0);
}

void Model::draw (const mat4& ModelMatrix, Framebuffer* shadowmap) {
  glBindVertexArray (vao);
  root.draw (ModelMatrix, shadowmap);
  glBindVertexArray (0);
}


SMesh::SMesh (DeferredModel& model, const char* filePath) {
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(filePath, 
    aiProcess_Triangulate |
    aiProcess_PreTransformVertices
  );

  if (!scene) {
    error("failed to open ", filePath);
    return;
  }

  if (scene->mNumMeshes == 0) {
    error("malformed file ", filePath);
    return;
  }

  const aiMesh* mesh = scene->mMeshes[0];
  startIndex = model.vertices;

  for (unsigned int j = 0; j < mesh->mNumFaces; j += 1) {
    const aiFace& face = mesh->mFaces[j];

    for (int k = 0; k < 3; k += 1) {
      if (face.mNumIndices < 3) break;

      if ( mesh->HasPositions() &&
           mesh->HasTextureCoords(0) &&
           mesh->HasNormals()) {

        size += 1;
        model.vertices += 1;

        model.data.push_back(mesh->mVertices[face.mIndices[k]].x);
        model.data.push_back(mesh->mVertices[face.mIndices[k]].y);
        model.data.push_back(mesh->mVertices[face.mIndices[k]].z);

        model.data.push_back(mesh->mTextureCoords[0][face.mIndices[k]].x);
        model.data.push_back(mesh->mTextureCoords[0][face.mIndices[k]].y);

        model.data.push_back(mesh->mNormals[face.mIndices[k]].x);
        model.data.push_back(mesh->mNormals[face.mIndices[k]].y);
        model.data.push_back(mesh->mNormals[face.mIndices[k]].z);
      }
    }
  }

  aiString texpath;
  aiReturn hasTexture = scene->mMaterials[mesh->mMaterialIndex]->
                        GetTexture(aiTextureType_DIFFUSE, 0, &texpath);
  if (hasTexture == AI_SUCCESS) {
    std::string s = texpath.C_Str();
    s = "../media/models/" + s;
    texture = Texture(s, Texture::TEXTURE);
  }
}

void SMesh::draw (const mat4& modelMatrix) {
  texture.bind(1);
  Model::program.setUniform("model", modelMatrix);
  glDrawArrays(GL_TRIANGLES, startIndex, size);
}

void SMesh::draw (const mat4& modelMatrix, Framebuffer* shadowmap) {
  shadowmap->program->setUniform ("model", modelMatrix);
  glDrawArrays (GL_TRIANGLES, startIndex, size);
}

void SMesh::draw (const mat4& modelMatrix, Program* program, bool bindTex) {
  if (bindTex)
    texture.bind(1);
  program->setUniform ("model", modelMatrix);
  glDrawArrays (GL_TRIANGLES, startIndex, size);
}

void Model::setMatrix(std::string s, mat4 m) {
  program.setUniform(s, m);
}
