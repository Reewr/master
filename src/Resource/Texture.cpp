#include "Texture.hpp"
#include "../Utils/CFG.hpp"
#include "../Utils/Utils.hpp"

#include <SOIL.h>
#include <fstream>

using mmm::vec2;

std::map<unsigned int, GLuint> Texture::activeTextures;
GLuint Texture::activeTexture = 0;

Texture::Texture()
    : Logging::Log("Texture")
    , mMode(0)
    , mTextureId(0)
    , mSamplerId(0)
    , mSize(0, 0) {}

void Texture::unload() {
  mLog->debug("Unloading {}", mFilename);

  if (activeTexture == mTextureId)
    activeTexture = 0;

  for (auto a : activeTextures)
    if (a.second == mTextureId)
      a.second = 0;

  if (mTextureId != 0)
    glDeleteTextures(1, &mTextureId);

  if (mSamplerId != 0)
    glDeleteSamplers(1, &mSamplerId);

  mTextureId = 0;
  mSamplerId = 0;
}

Texture::~Texture() {
  if (mLoaded)
    unload();
}

const vec2& Texture::getSize() const {
  return mSize;
}

bool Texture::isActive(unsigned int textureUnit) const {
  if (activeTextures[textureUnit] == 0)
    return false;

  return activeTextures[textureUnit] == mTextureId;
}

void Texture::bind(unsigned int textureUnit) {
  if (isActive(textureUnit))
    return;

  activeTextures[textureUnit] = mTextureId;
  glActiveTexture(GL_TEXTURE0 + textureUnit);
  glBindTexture(GL_TEXTURE_2D, mTextureId);
  glBindSampler(textureUnit, mSamplerId);
}

void Texture::unbind(unsigned int textureUnit) {
  if (activeTextures[textureUnit] == 0)
    return;

  activeTextures[textureUnit] = 0;
  glActiveTexture(GL_TEXTURE0 + textureUnit);
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindSampler(textureUnit, 0);
}

GLuint Texture::getGLTexture() {
  return mTextureId;
}

GLuint Texture::generateGLTexture() {
  if (mTextureId == 0) {
    glGenTextures(1, &mTextureId);
    glGenSamplers(1, &mSamplerId);
  }

  return mTextureId;
}

bool Texture::load(ResourceManager*) {
  mLog->debug("Loading {}", mFilename);

  mMode = TEXTURE;
  glGenTextures(1, &mTextureId);
  glGenSamplers(1, &mSamplerId);
  bind(0);

  if (mFilename.substr(mFilename.size() - 4) == ".dds")
    loadDDS();
  else
    loadTexture();

  return true;
}

bool Texture::loadTexture() {
  int            width, height;
  unsigned char* image;

  image =
    SOIL_load_image(mFilename.c_str(), &width, &height, 0, SOIL_LOAD_RGBA);

  if (image == 0)
    throw std::runtime_error("Failed to load SOIL_IMAGE");

  glTexImage2D(GL_TEXTURE_2D,
               0,
               GL_RGBA8,
               width,
               height,
               0,
               GL_RGBA,
               GL_UNSIGNED_BYTE,
               image);

  if (!Utils::getGLError())
    throw std::runtime_error("Failed to load SOIL_IMAGE");

  if (mMode == TEXTURE) {
    generateMipmaps();
    linearMipmap();
    // setSamplerFiltering(GL_TEXTURE_MAX_ANISOTROPY_EXT, cfg->graphics.aniso);
  }
  if (mMode == GUI) {
    linear();
    clampToEdge();
  }

  SOIL_free_image_data(image);
  mSize = vec2(width, height);
  return true;
}

bool Texture::loadDDS() {
  mMode = DDS;
  mTextureId =
    SOIL_load_OGL_texture(mFilename.c_str(),
                          SOIL_LOAD_AUTO,
                          mTextureId,
                          SOIL_FLAG_MIPMAPS | SOIL_FLAG_COMPRESS_TO_DXT);

  if (mTextureId == 0 || !Utils::getGLError())
    throw std::runtime_error("Failed to load DDS image");

  linearMipmap();
  // if(cfg->graphics.aniso >= 1)
  //  setSamplerFiltering(GL_TEXTURE_MAX_ANISOTROPY_EXT, cfg->graphics.aniso);
  mSize = vec2();
  return true;
}

unsigned char* Texture::loadEmptyTexture(const vec2& size, bool isReturn) {
  const int      arraySize = size.x * size.y;
  unsigned char* texData   = new unsigned char[arraySize];

  for (int i   = 0; i < arraySize; i++)
    texData[i] = 255;

  createTexture(size, texData, GL_RED);
  generateMipmaps();
  linear();

  mSize = size;

  if (isReturn)
    return texData;

  delete[] texData;
  return NULL;
}

bool Texture::createTexture(const vec2&    s,
                            unsigned char* data,
                            GLenum         color,
                            GLenum         inColor) {
  static unsigned int nameNumber = 0;
  nameNumber++;
  mFilename = "createTexture" + std::to_string(nameNumber);
  mMode     = EMPTY;
  inColor   = color == GL_DEPTH_COMPONENT ? GL_DEPTH_COMPONENT : inColor;

  glGenTextures(1, &mTextureId);
  glGenSamplers(1, &mSamplerId);
  bind(0);

  glTexImage2D(GL_TEXTURE_2D,
               0,
               color,
               s.x,
               s.y,
               0,
               inColor,
               GL_UNSIGNED_BYTE,
               data);

  if (!Utils::getGLError())
    throw std::runtime_error("createTexture failed. View OpenGL error above");

  mSize = s;
  return true;
}

void Texture::changeSubTex(const vec2&    pos,
                           const vec2&    size,
                           unsigned char* change) {
  bind(0);
  glTexSubImage2D(GL_TEXTURE_2D,
                  0,
                  pos.x,
                  pos.y,
                  size.x,
                  size.y,
                  GL_RED,
                  GL_UNSIGNED_BYTE,
                  change);
}

void Texture::saveTexture(std::string fname, const vec2& size) {
  const long     imageSize = size.x * size.y * 3;
  unsigned char* data      = new unsigned char[imageSize];
  bind(0);
  glGetTexImage(GL_TEXTURE_2D, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

  int xa = (int) size.x % 256;
  int xb = (size.x - xa) / 256;
  int ya = (int) size.y % 256;
  int yb = (size.y - ya) / 256;

  // clang-format off
  unsigned char header[18] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                               (unsigned char) xa, (unsigned char) xb,
                               (unsigned char) ya, (unsigned char) yb,
                               24, 0 };
  // clang-format on

  for (unsigned int i = 0; i < imageSize; i += 3) {
    data[i + 1] = data[i + 1] + 25;
    data[i + 2] = data[i + 2] + 25;
    data[i]     = data[i] + 25;
  }

  std::fstream f(fname, std::ios::out | std::ios::binary);
  f.write(reinterpret_cast<char*>(header), sizeof(char) * 18);
  f.write(reinterpret_cast<char*>(data), sizeof(char) * imageSize);
  f.close();
  delete[] data;
  data = NULL;
  mLog->debug("Saved texture to: '{}'", fname);
}

Texture& Texture::linear() {
  setSamplerFiltering(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  setSamplerFiltering(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  return *this;
}

Texture& Texture::nearest() {
  setSamplerFiltering(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  setSamplerFiltering(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  return *this;
}

Texture& Texture::linearMipmap() {
  setSamplerFiltering(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  setSamplerFiltering(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  return *this;
}

Texture& Texture::nearestMipmap() {
  setSamplerFiltering(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  setSamplerFiltering(GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
  return *this;
}

Texture& Texture::linearNearestMipmap() {
  setSamplerFiltering(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  setSamplerFiltering(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
  return *this;
}

Texture& Texture::clampToEdge() {
  setSamplerFiltering(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  setSamplerFiltering(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  return *this;
}

void Texture::setSamplerFiltering(GLenum param, GLenum value) {
  if (mTextureId == 0 || mSamplerId == 0)
    return;

  glSamplerParameteri(mSamplerId, param, value);

  if (!Utils::getGLError())
    throw std::runtime_error("Failed to set Sampler Parameteri");
}

Texture& Texture::generateMipmaps(GLenum target) {
  if (!isActive(0) && target != GL_TEXTURE_CUBE_MAP)
    bind(0);

  glGenerateMipmap(target);
  if (!Utils::getGLError())
    throw std::runtime_error("Failed to generateMipmaps");

  return *this;
}

void Texture::unbindAll() {
  for (auto k : activeTextures) {
    if (k.second != 0) {
      glActiveTexture(GL_TEXTURE0 + k.first);
      glBindTexture(GL_TEXTURE_2D, 0);
      glBindSampler(0, 0);
      k.second = 0;
    }
  }
}
