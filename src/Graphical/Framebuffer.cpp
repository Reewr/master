#include "Framebuffer.hpp"

#include "../GlobalLog.hpp"
#include "../OpenGLHeaders.hpp"
#include <fstream>

#include "../GLSL/Program.hpp"
#include "../Resource/Texture.hpp"
#include "../Shape/GL/Rectangle.hpp"
#include "../Utils/CFG.hpp"
#include "../Utils/Utils.hpp"

CFG*        Framebuffer::cfg   = NULL;
std::string Framebuffer::ssLoc = "";
int         Framebuffer::numSS = 0;

using mmm::vec2;

Framebuffer::Framebuffer()
    : Logging::Log("Framebuffer")
    , mIsDepth(false)
    // , mIsOwnProgram(false)
    // , mNeedsDrawing(false)
    , mIsBound(false)
    , mFrameSize(0, 0)
    , mQuad(nullptr)
    , mProgram(nullptr)
    , mTexture(nullptr)
    , mFrameBuffer(0) {}

Framebuffer::Framebuffer(std::shared_ptr<Program> program,
                         const mmm::vec2&         size,
                         bool                     depth)
    : Logging::Log("Framebuffer")
    , mIsDepth(depth)
    // , mIsOwnProgram(false)
    // , mNeedsDrawing(false)
    , mIsBound(false)
    , mFrameSize(size)
    , mQuad(nullptr)
    , mProgram(program)
    , mTexture(nullptr)
    , mFrameBuffer(0) {
  setup();
}

Framebuffer::~Framebuffer() {
  glDeleteFramebuffers(1, &mFrameBuffer);

  delete mTexture;
  delete mQuad;
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
  mQuad    = new GLRectangle(vec2(), mFrameSize);

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
std::shared_ptr<Program> Framebuffer::program() {
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
GLRectangle* Framebuffer::quad() {
  return mQuad;
}

///**
// * @brief
// *   Readies the Framebuffer for drawing by setting
// *   the position and figuring out how much of the texture
// *   to draw.
// *
// *   This does not bind the texture
// *
// * @param position
// */
// void Framebuffer::activateDraw(const vec2& position) {
//  failCheck();
//  mTexture->recalculateGeometry(Rectangle(position, mFrameSize));
//  mNeedsDrawing = true;
//}
//
///**
// * @brief
// *   Binds the drawing program and draws the texture
// */
// void Framebuffer::draw() {
//  failCheck();
//  if (!mNeedsDrawing)
//    activateDraw();
//  drawProgram->bind();
//  mTexture->draw();
//}

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
void Framebuffer::bind(std::shared_ptr<Program> program) {
  if (program == nullptr)
    throw std::runtime_error("Program is null");

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
void Framebuffer::nonClearBind(std::shared_ptr<Program> program) {
  if (program == nullptr)
    throw std::runtime_error("Pointer to program is null");

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

void Framebuffer::doQuad(GLRectangle*                 inQuad,
                         std::shared_ptr<Program>     program,
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

void Framebuffer::doQuad(GLRectangle*                 inQuad,
                         const std::vector<Texture*>& t,
                         unsigned int                 tStart) {
  doQuad(inQuad, mProgram, t, tStart);
}

void Framebuffer::doQuad(std::shared_ptr<Program>     p,
                         const std::vector<Texture*>& t,
                         unsigned int                 tStart) {
  doQuad(mQuad, p, t, tStart);
}

void Framebuffer::doQuad(const std::vector<Texture*>& t, unsigned int tStart) {
  doQuad(mQuad, mProgram, t, tStart);
}

void Framebuffer::nonClearQuad(GLRectangle*                 inQuad,
                               std::shared_ptr<Program>     p,
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

void Framebuffer::nonClearQuad(GLRectangle*                 inQuad,
                               const std::vector<Texture*>& t,
                               unsigned int                 tStart) {
  nonClearQuad(inQuad, mProgram, t, tStart);
}

void Framebuffer::nonClearQuad(std::shared_ptr<Program>     p,
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

void Framebuffer::queueQuad(GLRectangle*                 inQuad,
                            std::shared_ptr<Program>     p,
                            const std::vector<Texture*>& t,
                            unsigned int                 tStart) {
  if (!mIsBound)
    throw std::runtime_error("Tried using queue without starting");

  p->bind();

  for (unsigned int i = 0; i < t.size(); i++) {
    t[i]->bind(tStart);
    tStart++;
  }

  inQuad->draw();
}

void Framebuffer::queueQuad(GLRectangle*                 inQuad,
                            const std::vector<Texture*>& t,
                            unsigned int                 tStart) {
  queueQuad(inQuad, mProgram, t, tStart);
}
void Framebuffer::queueQuad(std::shared_ptr<Program>     p,
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

std::vector<float> Framebuffer::getPixels(const Rectangle& r, GLenum type) {
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
  return getPixels(Rectangle(pos, vec2(1, 1)), type)[0];
}

void Framebuffer::printPixels(const Rectangle& r,
                              GLenum           type,
                              std::string      name) {
  const int          mult = (type == GL_RGBA) ? 4 : (type == GL_RGB) ? 3 : 1;
  std::vector<float> data = getPixels(r, type);
  mLog->debug("{}:", name);
  for (unsigned int i = 0; i < data.size(); i += mult) {
    std::string output = "  [";
    for (int j = 0; j < mult; j++)
      output += std::to_string(data[i + j]) + ((j + 1 < mult) ? ", " : "");
    output += "]";
    mLog->debug(output);
  }
}

void Framebuffer::save(std::string filename) {
  mTexture->saveTexture(filename, mFrameSize);
}

/* void Framebuffer::clear() { */
/*   clearProgram->setUniform("screenRes", mFrameSize); */
/*   doQuad(clearProgram, {}); */
/* } */

/* void Framebuffer::copy(Texture* toCopy) { */
/*   copyProgram->setUniform("screenRes", mFrameSize); */
/*   doQuad(copyProgram, { toCopy }); */
/* } */

bool Framebuffer::isInitialized() {
  if (mFrameBuffer != 0 && mTexture != NULL && mProgram != NULL)
    return true;
  return false;
}

void Framebuffer::init(CFG* c, std::string screenshotLoc) {
  Framebuffer::cfg = c;
  ssLoc            = screenshotLoc;
  numSS            = 0;
}

/**
 * @brief
 *   Takes a screenshot of what is currently being displayed on
 *   the screen. This function will not overwrite previous images
 *   and will try to save them to a filename that is as follows:
 *
 *   `N_XxY.tga`
 *
 *   where
 *
 *   - `N` is a number given to it to find a unique name, will
 *     be incremented each time a new screenshot is saved.
 *   - `X` is the resolution in the X axis
 *   - `Y` is the resolution in the Y axis
 */
void Framebuffer::takeScreenshot() {
  int  i   = 0;
  vec2 res = cfg->graphics.res;

  std::string filename = "";
  std::string endStr =
    std::to_string((int) res.x) + "x" + std::to_string((int) res.y) + ".tga";

  // find a filename that isn't already taken
  do {
    filename = std::to_string(i) + "_" + endStr;
  } while (std::ifstream(filename));

  // create a buffer to contain this in. RGB colors for the whole screen
  // therefore x*y*3
  const long     imageSize = res.x * res.y * 3;
  unsigned char* data      = new unsigned char[imageSize];

  // make sure the framebuffer is the actual screen
  // and not some other framebuffer then read pixels
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glReadPixels(0, 0, res.x, res.y, GL_BGR, GL_UNSIGNED_BYTE, data);

  // store them as values between 0 and 255
  int xa = (int) res.x % 256;
  int xb = (res.x - xa) / 256;
  int ya = (int) res.y % 256;
  int yb = (res.y - ya) / 256;

  // clang-format off
  unsigned char header[18] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                               (unsigned char) xa, (unsigned char) xb,
                               (unsigned char) ya, (unsigned char) yb,
                               24, 0 };
  // clang-format on

  // finally write to file
  std::fstream f(filename, std::ios::out | std::ios::binary);
  f.write(reinterpret_cast<char*>(header), sizeof(char) * 18);
  f.write(reinterpret_cast<char*>(data), sizeof(char) * imageSize);
  f.close();

  // clean up
  delete[] data;
  data = nullptr;
  debug("Saved screenshot to: '", filename, "'");
}

void Framebuffer::printFramebufferLimits() {
  int res;
  glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &res);
  debug("Framebuffer - Max Color Attachments: ", res);
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
    throw std::runtime_error("Using uninitialized framebuffer!");
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
    throw std::runtime_error("Framebuffer is not complete.");
  }
}
