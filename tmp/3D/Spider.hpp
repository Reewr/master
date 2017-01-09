#ifndef SPIDER_HPP
#define SPIDER_HPP 

#include <Math/Math.hpp>
#include <Animation/Animation.hpp>

#include <OGL/OGL-3-3.hpp>
#include <string>

class Texture;
class Program;
class Framebuffer;
struct SMesh;

namespace Terrain {
  class Terrain;
}

struct Spider {

  typedef Animation::Frame<97> Frame;
  typedef Animation::Data<97>  Data;
  typedef Animation::Anim<97>  Anim;

  struct State {
    
    union {
      Frame frame;
      struct {
        struct {
          vec3 pos;
          vec3 rot;
        } vec;
        Animation::Frame<67> absolute;
      } part;
    };

    struct Transform {
      const mat4 s = scale(0.1, 0.1, 0.1);
            mat4 r;
            mat4 p;
            mat4 h;

      constexpr operator mat4() {return h * s * p * r;}
    } transform;

    struct IK {
      vec3 l1, l0;
      vec3 l3, l2;
      vec3 l5, l4;
      vec3 l7, l6;
    } ik;

    float height;
    float dir;
    vec3  pos;
    vec3  next;
    float anxiety = 1;
    float speed = 0;

    struct {
      bool idle    = false;
      bool walking = true;
    } is;

    State ();
  } state;

  Spider (vec2 pos, Terrain::Terrain* t);

  void update (float dt, Terrain::Terrain* t);
  void moveTo (const vec3& v);

  void draw (Texture* shadowTex);
  void draw (Framebuffer* shadowmap);

  static void init ();
  static void deinit ();

private:
  float theta         () const;
  void  updateLegs    (Terrain::Terrain* t);

  void draw           (Program* program, bool bindTex);
  void drawLeg        (int i, mat4 m, Program* program, bool bindTex);
  void drawUpperBody  (mat4 m, Program* program, bool bindTex);
  void drawLowerBody  (mat4 m, Program* program, bool bindTex);

  // model geometry
  static GLuint vbo;
  static GLuint vao;

  static std::map<std::string, SMesh*> meshes;
  static std::map<std::string, Data*>  animation_data;

  // animations
  Anim* current;
  Anim  tmp_anim;
  Anim  breathing;
  Anim  ik;
  Anim  move;
};

#endif