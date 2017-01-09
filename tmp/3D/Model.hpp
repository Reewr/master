#ifndef MODEL_HPP
#define MODEL_HPP 

#include <vector>

#include <Math/Math.hpp>
#include <Graphical/Texture.hpp>
#include <GLSL/Program.hpp>

#include <OGL/OGL-3-3.hpp>


struct aiScene;
struct aiNode;
struct CFG;
class Framebuffer;

struct Mesh {
  std::vector<Mesh> children;
  mat4 transform;
  int startIndex = 0;
  int size = 0;
  Texture texture;

  Mesh () {}

  template <typename ModelT>
  Mesh (ModelT& model, const aiScene* scene, const aiNode* node);

  void draw (const mat4& ModelMatrix);
  void draw (const mat4& ModelMatrix, Framebuffer* shadowmap);
};

struct Model {
  Mesh root;
  GLuint vbo = 0;
  GLuint vao = 0;

  std::vector<float> data;
  int vertices = 0;

  Model (const char* filePath);
  ~Model ();

  void draw (const mat4& ModelMatrix);
  void draw (const mat4& ModelMatrix, Framebuffer* shadowmap);
  void setMatrix (std::string s, mat4 m);

  static void init(CFG* cfg);
  static void deinit();
  static Program program;
};

struct DeferredModel {
  std::vector<float> data;
  int vertices = 0;
};

struct SMesh {
  Texture texture;
  int startIndex = 0;
  int size = 0;

  SMesh (DeferredModel& model, const char* filePath);
  void draw (const mat4& modelMatrix);
  void draw (const mat4& modelMatrix, Framebuffer* shadowmap);
  void draw (const mat4& modelMatrix, Program* program, bool bindTex);
};

#endif
