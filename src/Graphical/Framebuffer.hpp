#ifndef GRAPHICAL_FRAMEBUFER_HPP
#define GRAPHICAL_FRAMEBUFER_HPP

#include <string>
#include <vector>

#include "../OpenGLHeaders.hpp"

#include "../Math/MathCD.hpp"

class Program;
class Texture;
class CFG;

namespace GL {
class Rectangle;
}

class Framebuffer {
public:
  Framebuffer();

  //! Create framebuffer using an already defined program together
  //! with a specific size
  Framebuffer(Program* p, const vec2& size, bool depth = false);

  //! Create a framebuffer using a GLSL program that has yet to be loaded
  //! from file and only contains a vertex shader.
  //! The size is uniform in both X and Y.
  Framebuffer(std::string vs, std::string fs, int size, bool depth = false);

  //! Creates a framebuffer using a GLSL program that has yet to be loaded
  //! from file and contains both the vertex and fragment shader within one
  //! file
  Framebuffer(std::string vsfs, const vec2& frameSize, bool depth = false);

  //! Creates a framebuffer using a GLSL program that has yet to be loaded
  //! from two files, one with vertex shader and the other with fragment
  //! shader
  Framebuffer(std::string vs,
              std::string fs,
              const vec2& frameSize,
              bool        depth = false);

  ~Framebuffer();

  //! Readies the Framebuffer for drawing by setting
  //! the position and figuring out how much of the texture
  //! to draw
  void activateDraw(const vec2& pos = vec2());

  //! Binds the drawing program and draws the texture
  void draw();

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

  void doQuad(GL::Rectangle*               inQuad,
              Program*                     p,
              const std::vector<Texture*>& t,
              unsigned int                 tStart = 0);

  void doQuad(GL::Rectangle*               inQuad,
              const std::vector<Texture*>& t,
              unsigned int                 tStart = 0);
  void
  doQuad(Program* p, const std::vector<Texture*>& t, unsigned int tStart = 0);
  void doQuad(const std::vector<Texture*>& t, unsigned int tStart = 0);

  void nonClearQuad(GL::Rectangle*               inQuad,
                    Program*                     p,
                    const std::vector<Texture*>& t,
                    unsigned int                 tStart = 0);
  void nonClearQuad(GL::Rectangle*               inQuad,
                    const std::vector<Texture*>& t,
                    unsigned int                 tStart = 0);
  void nonClearQuad(Program*                     p,
                    const std::vector<Texture*>& t,
                    unsigned int                 tStart = 0);
  void nonClearQuad(const std::vector<Texture*>& t, unsigned int tStart = 0);

  void queueStart();
  void queueQuad(GL::Rectangle*               inQuad,
                 Program*                     p,
                 const std::vector<Texture*>& t,
                 unsigned int                 tStart = 0);
  void queueQuad(GL::Rectangle*               inQuad,
                 const std::vector<Texture*>& t,
                 unsigned int                 tStart = 0);
  void queueQuad(Program*                     p,
                 const std::vector<Texture*>& t,
                 unsigned int                 tStart = 0);
  void queueQuad(const std::vector<Texture*>& t, unsigned int tStart = 0);
  void queueEnd();

  std::vector<float> getPixels(const Rect& r, GLenum type = GL_RED);
  float getPixel(const vec2& pos, GLenum type = GL_RED);
  void printPixels(const Rect& r,
                   GLenum      type = GL_RED,
                   std::string name = "Framebuffer");
  void clear();
  void save(std::string filename);
  void copy(Texture* toCopy);

  Program*       program();
  Texture*       texture();
  GL::Rectangle* quad();

  bool isInitialized();

  static void init(CFG*        cfg,
                   std::string screenshotLoc = "./media/Screenshots/");
  static void deinit();
  static void initScreenshot();
  static void takeScreenshot();
  static void printFramebufferLimits();

  static Program* copyProgram;
  static Program* clearProgram;

private:
  void setup();
  /* void color(); */
  /* void depth(); */

  bool failCheck();
  void checkFramebuffer();

  bool mIsDepth;
  bool mIsOwnProgram;
  bool mNeedsDrawing;
  bool mIsBound;

  vec2 mFrameSize;

  GL::Rectangle* mQuad;
  Program*       mProgram;
  Texture*       mTexture;
  GLuint         mFrameBuffer;

  static int         numSS;
  static std::string ssLoc;
  static Program*    drawProgram;
  static CFG*        cfg;
};

#endif