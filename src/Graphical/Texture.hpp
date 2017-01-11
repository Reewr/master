#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <map>
#include <string>
#include <vector>

#include "../OpenGLHeaders.hpp"

#include "../Math/MathCD.hpp"
#include "GL/Rectangle.hpp"

class CFG;

//! Texture contains information about the loaded texture,
//! can load .PNG & .dds. Can also draw simple squared texture
struct Texture {

  //! Textures to load - MODE
  enum { GUI, TEXTURE, DDS, CUBE, MAP, EMPTY };
  enum { CUBENORMAL, CUBEREFLECTION };

  struct TexPair {
    TexPair();
    std::map<int, std::map<int, vec4>> imageMap;
    GLuint textureID;
    GLuint sampler;
    int    count;
    vec2   size;
  };

  //! Constructor - does nothing
  Texture();

  //! Loads the texture at once by calling loadTexture
  Texture(std::string filename, int mode = 0);

  //! Deconstructor - Deletes the texture
  ~Texture();

  //! Copy constructor
  Texture(const Texture& tex);

  //! Equals
  Texture& operator=(const Texture& tex);

  //! Loads a texture using SOIL. Can load .PNG
  bool loadTexture(std::string filename, bool isGUI = false);

  //! loads a cubemap texture using SOIL. Can load .PNG
  bool loadCubeTexture(const std::vector<std::string>& filenames);

  //! Loads a .png file, but only reads the colors (RBGA) into imageMap.
  bool loadMap(std::string filename);

  //! Loads a white texture into OpenGL with specified size -
  //! Returns a pointer. Needs to be deleted.
  unsigned char* loadEmptyTexture(const vec2& size, bool isReturn = false);

  //! Creates a texture out from data. NB: DOES NOT SET FILTERING
  bool createTexture(const vec2&    s,
                     unsigned char* data    = NULL,
                     GLenum         color   = GL_RGBA,
                     GLenum         inColor = GL_RGBA);

  //! Changes texture colors at specific position with size
  void changeSubTex(const vec2& pos, const vec2& size, unsigned char* change);

  //! Saves a texture a TGA file.
  void saveTexture(std::string filename, const vec2& size);

  //! frees the vector that contains the image data
  void freeImageMap();

  //! Binds the loaded texture so you can use it.
  //! Parameter: GL_TEXTUREX where X = [0-3]
  void bind(unsigned int textureUnit);
  void unbind(unsigned int textureUnit);

  //! Binds a cubemap texture so you can use it.
  //! Parameter: GL_TEXTUREX where X = [0-3]
  void bindCube(unsigned int textureUnit);

  //! Returns the size of the loaded texture
  vec2 getSize() const;

  //! Returns a vec4 of color values (0->255) (R, G, B, A);
  vec4 getPixelColors(int x, int y);

  //! Returns an OpenGL Texture number or 0 if not exists.
  GLuint getGLTexture();

  GLuint generateGLTexture(std::string filename);

  //! Checks if texture is active
  bool isActive(unsigned int textureUnit) const;

  //! Initiates the square to draw the texture in
  void recalculateGeometry(const Rect& r);

  //! Draws the texture in square set at recalculateGeometry.
  //! Does NOT bind Program. Has to be binded beforehand
  void draw();

  //! Filtering options - returns this reference so you can chain.
  Texture& linear();
  Texture& nearest();
  Texture& linearMipmap();
  Texture& nearestMipmap();
  Texture& linearNearestMipmap();
  Texture& clampToEdge();
  Texture& generateMipmaps(GLenum target = GL_TEXTURE_2D);

  //! Lets you individually set the parameter. Only takes openGL enums
  void setSamplerFiltering(GLenum param, GLenum value);

  //! Unbinds the current texture from OpenGL
  static void unbindAll();

  //! Initalizes the texture class with CFG so it can set texture settings
  static void init(CFG* c);

private:
  bool loadDDS();
  bool loadTexture();
  void loadCube();
  //! Throws new error if filepath does not exist.
  void imageExists(std::string filename);
  std::string filename;

  int           mode;
  GL::Rectangle rect;
  GLuint        VBO;
  GLuint        IBO;

  static std::map<std::string, TexPair> textures;
  static std::map<unsigned int, GLuint> activeTextures;
  static GLuint activeTexture;
  static CFG*   cfg;
};

#endif
