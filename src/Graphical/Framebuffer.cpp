#include "Framebuffer.hpp"

#include "../OpenGLHeaders.hpp"
#include <fstream>

#include "../GLSL/Program.hpp"
#include "../Math/Math.hpp"
#include "../Utils/CFG.hpp"
#include "../Utils/Utils.hpp"
#include "GL/Rectangle.hpp"
#include "Texture.hpp"


CFG*        Framebuffer::cfg          = NULL;
Program*    Framebuffer::drawProgram  = NULL;
Program*    Framebuffer::copyProgram  = NULL;
Program*    Framebuffer::clearProgram = NULL;
std::string Framebuffer::ssLoc        = "";
int         Framebuffer::numSS        = 0;

Framebuffer::Framebuffer() {
  mFrameBuffer  = 0;
  mTexture      = NULL;
  mProgram      = NULL;
  mQuad         = NULL;
  mIsOwnProgram = false;
  mIsBound      = false;
}

Framebuffer::Framebuffer(Program* p, const vec2& size, bool depth) {
  mIsDepth      = depth;
  mFrameSize    = size;
  mProgram      = p;
  mNeedsDrawing = false;
  mIsOwnProgram = false;
  mIsBound      = false;

  setup();
}

Framebuffer::Framebuffer(std::string vs, std::string fs, int size, bool depth) {
  mIsDepth      = depth;
  mFrameSize    = vec2(size, size);
  mNeedsDrawing = false;
  mIsOwnProgram = true;
  mIsBound      = false;
  mProgram      = new Program(fs, vs);
  setup();
}

Framebuffer::Framebuffer(std::string vs,
                         std::string fs,
                         const vec2& size,
                         bool        depth) {
  mIsDepth      = depth;
  mFrameSize    = size;
  mNeedsDrawing = false;
  mIsOwnProgram = true;
  mIsBound      = false;
  mProgram      = new Program(vs, fs);
  setup();
}

Framebuffer::Framebuffer(std::string vsfs, const vec2& size, bool depth) {
  mIsDepth      = depth;
  mFrameSize    = size;
  mNeedsDrawing = false;
  mIsOwnProgram = true;
  mIsBound      = false;
  mProgram      = new Program(vsfs);
  setup();
}

Framebuffer::~Framebuffer() {
  glDeleteFramebuffers(1, &mFrameBuffer);

  delete mTexture;
  delete mQuad;

  if (mIsOwnProgram)
    delete mProgram;
}

/**
 * @private
 * @brief
 *   Sets up the framebuffer. This is used by all
 *   the constructors and is therefore its own
 *   function.
 *
 *   Creates the texture and rectangle that this
 *   framebuffer will use. Also initializes the
 *   OpenGL framebuffer and binds it.
 */
void Framebuffer::setup() {
  mTexture = new Texture();
  mQuad    = new GL::Rectangle(vec2(), mFrameSize);

  GLenum type          = mIsDepth ? GL_DEPTH_COMPONENT : GL_RGBA32F;
  GLenum buffer        = mIsDepth ? GL_NONE : GL_COLOR_ATTACHMENT0;
  GLenum attachment    = mIsDepth ? GL_DEPTH_ATTACHMENT : GL_COLOR_ATTACHMENT0;
  GLenum drawBuffer[1] = { buffer };

  glGenFramebuffers(1, &mFrameBuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer);

  mTexture->createTexture(mFrameSize, NULL, type);
  mTexture->linear();
  mTexture->clampToEdge();

  if (!mIsDepth) {
    mProgram->setUniform("screenRes", mFrameSize);
    mTexture->generateMipmaps();
  }

  glFramebufferTexture2D(GL_FRAMEBUFFER,
                         attachment,
                         GL_TEXTURE_2D,
                         mTexture->getGLTexture(),
                         0);
  glDrawBuffers(1, drawBuffer);

  checkFramebuffer();

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * @brief
 *   Returns a pointer to the program that the framebuffer
 *   uses. Keep in mind that this program may not be owned
 *   by the frameBuffer.
 *
 * @return Program
 */
Program* Framebuffer::program() {
  return mProgram;
}

/**
 * @brief
 *   Returns a pointer to the texture that the framebuffer
 *   uses. This texture is what the framebuffer draws to
 *   when used.
 *
 * @return Texture
 */
Texture* Framebuffer::texture() {
  return mTexture;
}

/**
 * @brief
 *   Returns a pointer to the rectangle that defines the size
 *   or viewport of the framebuffer.
 *
 * @return Texture
 */
GL::Rectangle* Framebuffer::quad() {
  return mQuad;
}

/**
 * @brief
 *   Readies the Framebuffer for drawing by setting
 *   the position and figuring out how much of the texture
 *   to draw.
 *
 *   This does not bind the texture
 *
 * @param position
 */
void Framebuffer::activateDraw(const vec2& position) {
  failCheck();
  mTexture->recalculateGeometry(Rect(position, mFrameSize));
  mNeedsDrawing = true;
}

/**
 * @brief
 *   Binds the drawing program and draws the texture
 */
void Framebuffer::draw() {
  failCheck();
  if (!mNeedsDrawing)
    activateDraw();
  drawProgram->bind();
  mTexture->draw();
}

/**
 * @brief
 *   Binds the framebuffer and sets the viewport
 *   to the size defined by the frame size.
 *   If bindProgram is true, it also bounds
 *   the program
 *
 *   When using this function, this also clears the
 *   texture when binding the framebuffer
 *
 * @param bindProgram if true, binds the saved program
 */
void Framebuffer::bind(bool bindProgram) {
  failCheck();
  glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer);
  glViewport(0, 0, mFrameSize.x, mFrameSize.y);

  if (mIsDepth)
    glClear(GL_DEPTH_BUFFER_BIT);
  else
    glClear(GL_COLOR_BUFFER_BIT);
  glClearColor(0.0, 0.0, 0.0, 1.0);

  if (bindProgram)
    mProgram->bind();
  mIsBound = true;
}

/**
 * @brief
 *   Binds the framebuffer together with the given program.
 *   It also sets the viewport to the size defined by the frame
 *   size.
 *
 *   When using this function, this also clears the
 *   texture when binding the framebuffer
 *
 * @param program
 *   the program to use
 */
void Framebuffer::bind(Program* program) {
  if (program == nullptr)
    throw Error("Program is null");

  bind(false);
  program->bind();
}

/**
 * @brief
 *   Just like bind(), this binds the framebuffer
 *   and also binds the program if bindProgram is true.
 *
 *   However, unlike bind(), this does not clear the
 *   framebuffer when binding it
 *
 * @param bindProgram
 */
void Framebuffer::nonClearBind(bool bindProgram) {
  failCheck();
  glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer);
  glViewport(0, 0, mFrameSize.x, mFrameSize.y);

  if (bindProgram)
    mProgram->bind();

  mIsBound = true;
}

/**
 * @brief
 *   Just like bind(), this binds the framebuffer
 *   and also binds the program if bindProgram is true.
 *
 *   Just like, nonClearBind(), this does not clear the
 *   framebuffer when binding it. Instead of requiring a
 *   boolean telling it whether to bind the stored program,
 *   this function takes a program that it should bind
 *
 * @param bindProgram
 */
void Framebuffer::nonClearBind(Program* program) {
  if (program == nullptr)
    throw Error("Pointer to program is null");

  nonClearBind(false);
  program->bind();
}

/**
 * @brief
 *   This function is used to finalize the framebuffer,
 *   unbinding it and setting the viewport back to normal.
 */
void Framebuffer::finalize() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // I removed this as it gave a significant performance increase,
  // however, there may have been a reason to why this was done
  // after every framebuffer draw.
  /* glClearColor(1.0, 1.0, 1.0, 1.0); */
  /* glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); */

  glViewport(0, 0, cfg->graphics.res.x, cfg->graphics.res.y);
  mIsBound = false;
}

void Framebuffer::doQuad(GL::Rectangle*               inQuad,
                         Program*                     program,
                         const std::vector<Texture*>& t,
                         unsigned int                 textureStart) {
  bind(program);
  // bind the textures on different locations, based on tStart
  for (unsigned int i = 0; i < t.size(); i++) {
    t[i]->bind(textureStart);
    textureStart++;
  }

  inQuad->draw();
  finalize();
}

void Framebuffer::doQuad(GL::Rectangle*               inQuad,
                         const std::vector<Texture*>& t,
                         unsigned int                 tStart) {
  doQuad(inQuad, mProgram, t, tStart);
}

void Framebuffer::doQuad(Program*                     p,
                         const std::vector<Texture*>& t,
                         unsigned int                 tStart) {
  doQuad(mQuad, p, t, tStart);
}

void Framebuffer::doQuad(const std::vector<Texture*>& t, unsigned int tStart) {
  doQuad(mQuad, mProgram, t, tStart);
}

void Framebuffer::nonClearQuad(GL::Rectangle*               inQuad,
                               Program*                     p,
                               const std::vector<Texture*>& t,
                               unsigned int                 tStart) {
  nonClearBind(p);
  for (unsigned int i = 0; i < t.size(); i++) {
    t[i]->bind(tStart);
    tStart++;
  }
  inQuad->draw();
  finalize();
}

void Framebuffer::nonClearQuad(GL::Rectangle*               inQuad,
                               const std::vector<Texture*>& t,
                               unsigned int                 tStart) {
  nonClearQuad(inQuad, mProgram, t, tStart);
}

void Framebuffer::nonClearQuad(Program*                     p,
                               const std::vector<Texture*>& t,
                               unsigned int                 tStart) {
  nonClearQuad(mQuad, p, t, tStart);
}

void Framebuffer::nonClearQuad(const std::vector<Texture*>& t,
                               unsigned int                 tStart) {
  nonClearQuad(mQuad, mProgram, t, tStart);
}

void Framebuffer::queueStart() {
  bind(false);
}

void Framebuffer::queueQuad(GL::Rectangle*               inQuad,
                            Program*                     p,
                            const std::vector<Texture*>& t,
                            unsigned int                 tStart) {
  if (!mIsBound)
    throw Error("Tried using queue without starting");
  p->bind();
  for (unsigned int i = 0; i < t.size(); i++) {
    t[i]->bind(tStart);
    tStart++;
  }
  inQuad->draw();
}

void Framebuffer::queueQuad(GL::Rectangle*               inQuad,
                            const std::vector<Texture*>& t,
                            unsigned int                 tStart) {
  queueQuad(inQuad, mProgram, t, tStart);
}
void Framebuffer::queueQuad(Program*                     p,
                            const std::vector<Texture*>& t,
                            unsigned int                 tStart) {
  queueQuad(mQuad, p, t, tStart);
}
void Framebuffer::queueQuad(const std::vector<Texture*>& t,
                            unsigned int                 tStart) {
  queueQuad(mQuad, mProgram, t, tStart);
}

void Framebuffer::queueEnd() {
  finalize();
}

std::vector<float> Framebuffer::getPixels(const Rect& r, GLenum type) {
  int mult = 1;

  if (type == GL_RGBA) {
    mult = 4;
  } else if (type == GL_RGB) {
    mult = 3;
  }

  std::vector<float> data = std::vector<float>(r.size.x * r.size.y * mult);

  // bind the frame buffer and set viewport
  glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer);
  glViewport(0, 0, mFrameSize.x, mFrameSize.y);

  glReadPixels(r.topleft.x,
               r.topleft.y,
               r.size.x,
               r.size.y,
               mIsDepth ? GL_DEPTH_COMPONENT : type,
               GL_FLOAT,
               &data[0]);

  // unbind it and viewport back to normal
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, cfg->graphics.res.x, cfg->graphics.res.y);

  return data;
}

float Framebuffer::getPixel(const vec2& pos, GLenum type) {
  return getPixels(Rect(pos, vec2(1, 1)), type)[0];
}

void Framebuffer::printPixels(const Rect& r, GLenum type, std::string name) {
  const int          mult = (type == GL_RGBA) ? 4 : (type == GL_RGB) ? 3 : 1;
  std::vector<float> data = getPixels(r, type);
  log(name, ":");
  for (unsigned int i = 0; i < data.size(); i += mult) {
    std::string output = "  [";
    for (int j = 0; j < mult; j++)
      output += Utils::toStr(data[i + j]) + ((j + 1 < mult) ? ", " : "");
    output += "]";
    log(output);
  }
}

void Framebuffer::save(std::string filename) {
  mTexture->saveTexture(filename, mFrameSize);
}

void Framebuffer::clear() {
  clearProgram->setUniform("screenRes", mFrameSize);
  doQuad(clearProgram, {});
}

void Framebuffer::copy(Texture* toCopy) {
  copyProgram->setUniform("screenRes", mFrameSize);
  doQuad(copyProgram, { toCopy });
}

bool Framebuffer::isInitialized() {
  if (mFrameBuffer != 0 && mTexture != NULL && mProgram != NULL)
    return true;
  return false;
}

void Framebuffer::init(CFG* cfg, std::string screenshotLoc) {
  Framebuffer::cfg = cfg;
  ssLoc            = screenshotLoc;
  numSS            = 0;
  drawProgram      = new Program("shaders/GUI/GUI.vsfs", 0);
  drawProgram->bindAttribs({ "position", "texcoord" }, { 0, 1 });
  drawProgram->link();
  drawProgram->setUniform("screenRes", cfg->graphics.res, "guiOffset", vec2());
  drawProgram->setUniform("inTexture", 0);

  clearProgram =
    new Program("shaders/Framebuffer.vs", "shaders/Utils/Clear.fs");
  copyProgram = new Program("shaders/Framebuffer.vs", "shaders/Utils/Copy.fs");
  copyProgram->setUniform("toCopy", 0);
  initScreenshot();
}

void Framebuffer::deinit() {
  delete drawProgram;
  delete clearProgram;
  delete copyProgram;
}

void Framebuffer::initScreenshot() {
  std::string name = Utils::toStr((int) cfg->graphics.res.x) + "x";
  name += Utils::toStr((int) cfg->graphics.res.y);
  name += " - ";
  while (true) {
    std::string test = ssLoc + name + Utils::toStr(numSS) + ".tga";
    if (!Utils::fileExists(test)) {
      log("Saving Screenshots to: " + test);
      return;
    }
    numSS += 1;
  }
}

void Framebuffer::takeScreenshot() {
  std::string filename = ssLoc + Utils::toStr((int) cfg->graphics.res.x) + "x";
  filename += Utils::toStr((int) cfg->graphics.res.y);
  filename += " - " + Utils::toStr(numSS) + ".tga";
  numSS++;
  vec2           size      = cfg->graphics.res;
  const long     imageSize = cfg->graphics.res.x * cfg->graphics.res.y * 3;
  unsigned char* data      = new unsigned char[imageSize];
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glReadPixels(0, 0, size.x, size.y, GL_BGR, GL_UNSIGNED_BYTE, data);
  int           xa         = (int) size.x % 256;
  int           xb         = (size.x - xa) / 256;
  int           ya         = (int) size.y % 256;
  int           yb         = (size.y - ya) / 256;
  unsigned char header[18] = { 0,
                               0,
                               2,
                               0,
                               0,
                               0,
                               0,
                               0,
                               0,
                               0,
                               0,
                               0,
                               (unsigned char) xa,
                               (unsigned char) xb,
                               (unsigned char) ya,
                               (unsigned char) yb,
                               24,
                               0 };

  std::fstream f(filename, std::ios::out | std::ios::binary);
  f.write(reinterpret_cast<char*>(header), sizeof(char) * 18);
  f.write(reinterpret_cast<char*>(data), sizeof(char) * imageSize);
  f.close();
  delete[] data;
  data = NULL;
  log("Saved screenshot to: '", filename, "'");
}

void Framebuffer::printFramebufferLimits() {
  int res;
  glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &res);
  log("Framebuffer - Max Color Attachments: ", res);
  // glGetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH, &res);
  // log("Framebuffer - Max Width: ", res);
  // glGetIntegerv(GL_MAX_FRAMEBUFFER_HEIGHT, &res);
  // log("Framebuffer - Max Height: ", res);
  // glGetIntegerv(GL_MAX_FRAMEBUFFER_SAMPLES, &res);
  // log("Framebuffer - Max Samples: ", res);
  // glGetIntegerv(GL_MAX_FRAMEBUFFER_LAYERS, &res);
  // log("Framebuffer - Max layers: ", res);
}

bool Framebuffer::failCheck() {
  if (!isInitialized())
    throw Error("Using unintialized framebuffer!");
  return true;
}

void Framebuffer::checkFramebuffer() {
  GLenum glCheck = glCheckFramebufferStatus(GL_FRAMEBUFFER);

  if (glCheck != GL_FRAMEBUFFER_COMPLETE) {
    switch (glCheck) {
      case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        error("Incomplete attachment");
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
        error("Missing attachment");
        break;
      case GL_FRAMEBUFFER_UNSUPPORTED:
        error("Not supported");
        break;
    }
    throw Error("Framebuffer is not complete.");
  }
}