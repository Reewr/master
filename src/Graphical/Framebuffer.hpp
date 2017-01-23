#pragma once

#include <string>
#include <vector>

#include "../OpenGLHeaders.hpp"

#include "../Math/Math.hpp"

class Program;
class Texture;
class CFG;
class Rectangle;
class GLRectangle;

class Framebuffer {
public:
  Framebuffer();

  //! Create framebuffer using an already defined program together
  //! with a specific size
  Framebuffer(Program* p, const vec2& size, bool depth = false);

  ~Framebuffer();

  //! Readies the Framebuffer for drawing by setting
  //! the position and figuring out how much of the texture
  //! to draw
  /* void activateDraw(const vec2& pos = vec2()); */

  //! Binds the drawing program and draws the texture
  /* void draw(); */

  //! Bind the framebuffer, also binds the program if
  //! bindProgram is true
  //! This clears the framebuffer
  void bind(bool bindProgram = true);

  //! Bind the framebuffer, also binds the given program
  //! This clears the framebuffer
  void bind(Program* p);

  //! Binds the framebuffer, but unlike nonClearBind() it
  //! does not clear.
  void nonClearBind(bool bindProgram = true);

  //! Binds the framebuffer and the given program. Does
  //! not clear the framebuffer
  void nonClearBind(Program* p);

  //! Finalizes the framebuffer, unbinding it
  //! and setting the viewport back to normal
  void finalize();

  void doQuad(GLRectangle*               inQuad,
              Program*                     p,
              const std::vector<Texture*>& t,
              unsigned int                 tStart = 0);

  void doQuad(GLRectangle*               inQuad,
              const std::vector<Texture*>& t,
              unsigned int                 tStart = 0);
  void
  doQuad(Program* p, const std::vector<Texture*>& t, unsigned int tStart = 0);
  void doQuad(const std::vector<Texture*>& t, unsigned int tStart = 0);

  void nonClearQuad(GLRectangle*               inQuad,
                    Program*                     p,
                    const std::vector<Texture*>& t,
                    unsigned int                 tStart = 0);
  void nonClearQuad(GLRectangle*               inQuad,
                    const std::vector<Texture*>& t,
                    unsigned int                 tStart = 0);
  void nonClearQuad(Program*                     p,
                    const std::vector<Texture*>& t,
                    unsigned int                 tStart = 0);
  void nonClearQuad(const std::vector<Texture*>& t, unsigned int tStart = 0);

  void queueStart();
  void queueQuad(GLRectangle*               inQuad,
                 Program*                     p,
                 const std::vector<Texture*>& t,
                 unsigned int                 tStart = 0);
  void queueQuad(GLRectangle*               inQuad,
                 const std::vector<Texture*>& t,
                 unsigned int                 tStart = 0);
  void queueQuad(Program*                     p,
                 const std::vector<Texture*>& t,
                 unsigned int                 tStart = 0);
  void queueQuad(const std::vector<Texture*>& t, unsigned int tStart = 0);
  void queueEnd();

  std::vector<float> getPixels(const Rectangle& r, GLenum type = GL_RED);
  float getPixel(const vec2& pos, GLenum type = GL_RED);
  void printPixels(const Rectangle& r,
                   GLenum      type = GL_RED,
                   std::string name = "Framebuffer");
  void save(std::string filename);
  /* void copy(Texture* toCopy); */
  /* void clear(); */

  Program*       program();
  Texture*       texture();
  GLRectangle* quad();

  bool isInitialized();

  static void init(CFG*        cfg,
                   std::string screenshotLoc = "./media/Screenshots/");
  static void initScreenshot();
  static void takeScreenshot();
  static void printFramebufferLimits();

private:
  void setup();

  bool failCheck();
  void checkFramebuffer();

  bool mIsDepth;
  bool mIsOwnProgram;
  bool mNeedsDrawing;
  bool mIsBound;

  vec2 mFrameSize;

  GLRectangle* mQuad;
  Program*       mProgram;
  Texture*       mTexture;
  GLuint         mFrameBuffer;

  static int         numSS;
  static std::string ssLoc;
  static CFG*        cfg;
};
