#include <Graphical/Framebuffer.hpp>
#include <Graphical/Texture.hpp>
#include <GLSL/Program.hpp>
#include <3D/Spider.hpp>
#include <3D/Model.hpp>
#include <Terrain/Terrain.hpp>
#include <Utils/Utils.hpp>

#include <GLFW/glfw3.h>

GLuint Spider::vbo = 0;
GLuint Spider::vao = 0;
std::map<std::string, SMesh*>        Spider::meshes;
std::map<std::string, Spider::Data*> Spider::animation_data;

void Spider::init () {
  DeferredModel model;

  std::vector<std::string> meshnames = {
    "abdomin", "abdomin_inner", "coxa", "femur", "femur_tibia_inner", "head",
    "head_disk", "head_inner", "hip", "joint", "joint_rot", "metatarsus",
    "neck", "patella", "patella_inner", "sternum_back", "sternum_front",
    "tarsus_bot", "tarsus_top", "thorax", "tibia", "trochanter",
  };
  
  for (const auto& s: meshnames)
    meshes[s] = new SMesh(model, ("../media/models/spider/"+s+".dae").c_str());

  log ("loading: Spider ", model.vertices, " vertices");

  glGenBuffers (1, &vbo);
  glBindBuffer (GL_ARRAY_BUFFER, vbo);
  glBufferData (GL_ARRAY_BUFFER, model.vertices * 8 * sizeof(float), &model.data[0], GL_STATIC_DRAW);

  glGenVertexArrays (1, &vao);
  glBindVertexArray (vao);
  glBindBuffer (GL_ARRAY_BUFFER, vbo);

  glEnableVertexAttribArray (0);
  glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(0));
  glEnableVertexAttribArray (1);
  glVertexAttribPointer (1, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
  glEnableVertexAttribArray (2);
  glVertexAttribPointer (2, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(5*sizeof(float)));

  glBindVertexArray(0);

  // Init animations
  std::vector<std::string> animnames = {
    "walk", "spinL", "spinR", "turnL", "turnR", "breathing",
    "l0", "l1", "l2", "l3", "l4", "l5", "l6", "l7",
  };
  for (const auto& s: animnames)
    animation_data[s] = new Data(("../media/models/spider/Anims/"+s+".anim").c_str());
}
void Spider::deinit () {
  if(!meshes.empty())
    for (const auto& m: meshes)
      delete m.second;

  if(vbo != 0)
    glDeleteBuffers(1, &vbo);
  if(vao != 0)
    glDeleteVertexArrays(1, &vao);

  // deinit animations
  for (const auto& d: animation_data)
    delete d.second;
}


Spider::State::State () : frame(0) {}

Spider::Spider (vec2 pos, Terrain::Terrain* t) {
  Anim walk  = animation_data["walk"];
  Anim spinL = animation_data["spinR"]; // miss named file
  Anim spinR = animation_data["spinL"]; // miss named file
  Anim turnR = animation_data["turnR"];
  Anim turnL = animation_data["turnL"];

  Anim l0    = animation_data["l0"];
  Anim l1    = animation_data["l1"];
  Anim l2    = animation_data["l2"];
  Anim l3    = animation_data["l3"];
  Anim l4    = animation_data["l4"];
  Anim l5    = animation_data["l5"];
  Anim l6    = animation_data["l6"];
  Anim l7    = animation_data["l7"];

  ik  = (l0 - l0[0.5])[([this](){
    return 0.5 + 3.3333333 * (state.ik.l0.y - state.height + 0 * state.part.absolute[19]);
  })] + (l1 - l1[0.5])[([this](){
    return 0.5 + 3.3333333 * (state.ik.l1.y - state.height + 0 * state.part.absolute[29]);
  })] + (l2 - l2[0.5])[([this](){
    return 0.5 + 3.3333333 * (state.ik.l2.y - state.height + 0 * state.part.absolute[39]);
  })] + (l3 - l3[0.5])[([this](){
    return 0.5 + 3.3333333 * (state.ik.l3.y - state.height + 0 * state.part.absolute[49]);
  })] + (l4 - l4[0.5])[([this](){
    return 0.5 + 3.3333333 * (state.ik.l4.y - state.height + 0 * state.part.absolute[59]);
  })] + (l5 - l5[0.5])[([this](){
    return 0.5 + 3.3333333 * (state.ik.l5.y - state.height + 0 * state.part.absolute[69]);
  })] + (l6 - l6[0.5])[([this](){
    return 0.5 + 3.3333333 * (state.ik.l6.y - state.height + 0 * state.part.absolute[79]);
  })] + (l7 - l7[0.5])[([this](){
    return 0.5 + 3.3333333 * (state.ik.l7.y - state.height + 0 * state.part.absolute[89]);
  })];

  Anim breath = animation_data["breathing"];
  breathing = loop (breath - breath[0]);

  Anim tL = turnL + [this](){
    return (clamp ( theta(), 32, 64) - 32.f) / 32.f;
  } * (spinL - turnL);
  Anim tR = turnR + [this](){
    return (clamp (-theta(), 32, 64) - 32.f) / 32.f;
  } * (spinR - turnR);

  move = loop (walk
    + [this](){return clamp ( theta(), 0, 10)/10.f;} * (tL - walk)
    + [this](){return clamp (-theta(), 0, 10)/10.f;} * (tR - walk)
  );

  state.height = t->findHeight(pos).y;
  state.transform.h = translate(0, state.height, 0);
  state.dir  = 0;
  state.pos  = vec3(pos.x, 0, pos.y);
  state.next = vec3(pos.x, 0, pos.y);

  tmp_anim = move (0);
  current  = &tmp_anim;
}

void Spider::update (float dt, Terrain::Terrain* t) {

  if (state.is.walking) {
    if (length2(state.next.xz - state.pos.xz) < 0.1) {
      state.is.walking = false;
      state.is.idle = true;
      tmp_anim = move(0);
      current  = &tmp_anim;
    } else {
      float theta = this->theta();

      float spin  = (clamp (std::abs (theta), 32, 64) - 32.f) / 32.f;
      float dir   = clamp (theta, -10, 10) + sign (theta) * spin * 22.f;
      float speed = 0.2 * (1 - spin) * dt;

      state.dir += dir * dt;
      vec4  pos  = rotate_y (state.dir) * vec4 (speed, 0, 0, 1);
      state.pos += pos.xyz;
    }
  }

  if (state.is.walking)
    state.anxiety = state.anxiety < 6 ? state.anxiety + 0.15 * dt : 6;
  else if (state.is.idle)
    state.anxiety = state.anxiety > 1 ? state.anxiety - 0.15 * dt : 1;

  state.frame  = (*current)(dt);
  state.frame += breathing (dt * state.anxiety);

  state.height = t->findHeight((state.pos + state.part.vec.pos).xz).y;
  state.transform.p = translate (10 * state.pos + state.part.vec.pos);
  state.transform.r = rotate (vec3(0, state.dir, 0) + state.part.vec.rot);
  state.transform.h = translate (0, state.height, 0);

  updateLegs(t);

  Frame tmp = ik(0);
  state.frame += tmp;
  state.transform.p *= translate (tmp[0], tmp[1], tmp[2]);
  state.transform.r *= rotate (tmp[3], tmp[4], tmp[5]);
}

void Spider::moveTo (const vec3& v) {
  state.next = v;

  if (!state.is.walking) {
    state.is.idle = false;
    state.is.walking = true;

    current = &move;
  }
}

float Spider::theta () const {
  vec3 v  = state.next - state.pos;
  float d = degrees (-atan2 (v.z, v.x)) - state.dir;
  return mod (d + 180.0f, 360.0f) - 180.0f;
}

void Spider::updateLegs (Terrain::Terrain* t) {
  vec4 pos = vec4(0,0,0,1);

  mat4 m   = state.transform;

  mat4 l0  = m * translate (state.part.absolute[18], 0*state.part.absolute[19], state.part.absolute[20]);
  mat4 l1  = m * translate (state.part.absolute[28], 0*state.part.absolute[29], state.part.absolute[30]);
  mat4 l2  = m * translate (state.part.absolute[38], 0*state.part.absolute[39], state.part.absolute[40]);
  mat4 l3  = m * translate (state.part.absolute[48], 0*state.part.absolute[49], state.part.absolute[50]);
  mat4 l4  = m * translate (state.part.absolute[58], 0*state.part.absolute[59], state.part.absolute[60]);
  mat4 l5  = m * translate (state.part.absolute[68], 0*state.part.absolute[69], state.part.absolute[70]);
  mat4 l6  = m * translate (state.part.absolute[78], 0*state.part.absolute[79], state.part.absolute[80]);
  mat4 l7  = m * translate (state.part.absolute[88], 0*state.part.absolute[89], state.part.absolute[90]);

  state.ik.l0 = t->findHeight ( (l0 * pos).xyz );
  state.ik.l1 = t->findHeight ( (l1 * pos).xyz );
  state.ik.l2 = t->findHeight ( (l2 * pos).xyz );
  state.ik.l3 = t->findHeight ( (l3 * pos).xyz );
  state.ik.l4 = t->findHeight ( (l4 * pos).xyz );
  state.ik.l5 = t->findHeight ( (l5 * pos).xyz );
  state.ik.l6 = t->findHeight ( (l6 * pos).xyz );
  state.ik.l7 = t->findHeight ( (l7 * pos).xyz );
}

void Spider::draw (Texture* shadowTex) {
  shadowTex->bind(0);
  draw(&Model::program, true);
}

void Spider::draw (Framebuffer* shadowmap) {
  draw(shadowmap->program, false);
}

void Spider::draw (Program* program, bool bindTex) {
  glBindVertexArray(vao);
  program->bind();

  mat4 m = state.transform;

  drawUpperBody(m, program, bindTex);
  drawLowerBody(m, program, bindTex);

  glBindVertexArray(0);
}

void Spider::drawLeg (int i, mat4 m, Program* program, bool bindTex) {
  i *= 10;
  m *= translate(0, 0, 0.4) * rotate_y(state.part.absolute[11 + i]);
  meshes["joint"]->draw(m * rotate_z(90), program, bindTex);
  meshes["trochanter"]->draw(m, program, bindTex);

  m *= translate(0, 0, 0.24) * rotate_x(state.part.absolute[12 + i]);
  meshes["joint"]->draw(m, program, bindTex);
  meshes["femur_tibia_inner"]->draw(m * translate(0, 0, 0.59), program, bindTex);
  meshes["femur"]->draw(m, program, bindTex);

  m *= translate(0, 0, 1.13) * rotate_x(-state.part.absolute[13 + i]);
  meshes["joint"]->draw(m, program, bindTex);
  meshes["patella_inner"]->draw(m * translate(0, 0, 0.4), program, bindTex);
  meshes["patella"]->draw(m, program, bindTex);

  m *= translate(0, 0, 0.8) * rotate_x(-state.part.absolute[14 + i]);
  meshes["joint"]->draw(m, program, bindTex);
  meshes["femur_tibia_inner"]->draw(m * translate(0, 0, 0.57) * rotate_x(180), program, bindTex);
  meshes["tibia"]->draw(m, program, bindTex);

  m *= translate(0, 0.02, 1.11) * rotate_z(state.part.absolute[15 + i]);
  meshes["joint_rot"]->draw(m, program, bindTex);
  meshes["metatarsus"]->draw(m, program, bindTex);

  m *= translate(0, 0, 0.1) * rotate_x(-state.part.absolute[16 + i]);
  meshes["joint"]->draw(m * scale(0.8, 0.8, 0.8), program, bindTex);
  meshes["tarsus_bot"]->draw(m * rotate_x( state.part.absolute[17 + i]), program, bindTex);
  meshes["tarsus_top"]->draw(m * rotate_x(-state.part.absolute[17 + i]), program, bindTex);
}
void Spider::drawUpperBody (mat4 m, Program* program, bool bindTex) {
  m *= rotate_y(state.part.absolute[0]);
  meshes["sternum_front"]->draw(m, program, bindTex);

  m *= translate(0.2, 0, 0) * rotate_z(state.part.absolute[1]);
  meshes["coxa"]->draw(m, program, bindTex);
  drawLeg(2, m, program, bindTex);
  glFrontFace(GL_CW);
  drawLeg(3, m * scale(1, 1, -1), program, bindTex);
  glFrontFace(GL_CCW);

  m *= rotate_z(-state.part.absolute[2]);
  meshes["thorax"]->draw(m, program, bindTex);

  m *= translate(0.4, 0, 0) * rotate_z(-state.part.absolute[3]);
  meshes["coxa"]->draw(m, program, bindTex);
  drawLeg(0, m, program, bindTex);
  glFrontFace(GL_CW);
  drawLeg(1, m * scale(1, 1, -1), program, bindTex);
  glFrontFace(GL_CCW);

  m *= rotate_z(-state.part.absolute[4]);
  meshes["neck"]->draw(m, program, bindTex);

  m *= translate(0.24, 0.3, 0) * rotate_y(state.part.absolute[5]);
  meshes["head_disk"]->draw(m, program, bindTex);

  m *= translate(0.07, 0, 0);
  meshes["head_inner"]->draw(m, program, bindTex);
  meshes["head"]->draw(m, program, bindTex);
}
void Spider::drawLowerBody (mat4 m, Program* program, bool bindTex) {
  m *= rotate_y(state.part.absolute[6]);
  meshes["sternum_back"]->draw(m, program, bindTex);

  m *= translate(-0.2, 0, 0) * rotate_z(state.part.absolute[7]);
  meshes["coxa"]->draw(m, program, bindTex);
  glFrontFace(GL_CW);
  drawLeg(4, m * scale(-1, 1, 1), program, bindTex);
  glFrontFace(GL_CCW);
  drawLeg(5, m * scale(-1, 1, -1), program, bindTex);
  glFrontFace(GL_CW);

  m *= rotate_z(state.part.absolute[8]);
  meshes["thorax"]->draw(m * scale(-1, 1, 1), program, bindTex);
  glFrontFace(GL_CCW);

  m *= translate(-0.4, 0, 0) * rotate_z(state.part.absolute[9]);
  meshes["coxa"]->draw(m, program, bindTex);
  glFrontFace(GL_CW);
  drawLeg(6, m * scale(-1, 1, 1), program, bindTex);
  glFrontFace(GL_CCW);
  drawLeg(7, m * scale(-1, 1, -1), program, bindTex);

  m *= rotate_z(state.part.absolute[10]);
  meshes["hip"]->draw(m, program, bindTex);

  m *= translate(-1, 0, 0);
  meshes["abdomin_inner"]->draw(m, program, bindTex);
  meshes["abdomin"]->draw(m, program, bindTex);
}
