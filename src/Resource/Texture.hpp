#pragma once

#include <map>
#include <mmm.hpp>
#include <string>

#include "../OpenGLHeaders.hpp"
#include "Resource.hpp"

class CFG;

//! Texture contains information about the loaded texture,
//! can load .PNG & .dds. Can also draw simple squared texture
class Texture : public Resource {
public:
  //! Textures to load - MODE
  enum { GUI, TEXTURE, DDS, CUBE, MAP, EMPTY };
  enum { CUBENORMAL, CUBEREFLECTION };

  //! Constructor - does nothing
  Texture();

  //! Deconstructor - Deletes the texture
  ~Texture();

  //! Loads a texture using SOIL. Can load .PNG
  bool load();

  void unload();

  //! Loads a white texture into OpenGL with specified size -
  //! Returns a pointer. Needs to be deleted.
  unsigned char* loadEmptyTexture(const mmm::vec2& size, bool isReturn = false);

  //! Creates a texture out from data. NB: DOES NOT SET FILTERING
  bool createTexture(const mmm::vec2& s,
                     unsigned char*   data    = NULL,
                     GLenum           color   = GL_RGBA,
                     GLenum           inColor = GL_RGBA);

  //! Changes texture colors at specific position with size
  void changeSubTex(const mmm::vec2& pos,
                    const mmm::vec2& size,
                    unsigned char*   change);

  //! Saves a texture a TGA file.
  void saveTexture(std::string filename, const mmm::vec2& size);

  //! Binds the loaded texture so you can use it.
  //! Parameter: GL_TEXTUREX where X = [0-3]
  void bind(unsigned int textureUnit);
  void unbind(unsigned int textureUnit);

  //! Returns the size of the loaded texture
  const mmm::vec2& getSize() const;

  //! Returns an OpenGL Texture number or 0 if not exists.
  GLuint getGLTexture();

  GLuint generateGLTexture();

  //! Checks if texture is active
  bool isActive(unsigned int textureUnit) const;

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

private:
  bool loadDDS();
  bool loadTexture();
  void loadCube();

  int       mMode;
  GLuint    mTextureId;
  GLuint    mSamplerId;
  mmm::vec2 mSize;

  static std::map<unsigned int, GLuint> activeTextures;
  static GLuint activeTexture;
};
