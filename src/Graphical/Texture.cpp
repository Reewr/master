#include <Graphical/Texture.hpp>
#include <Graphical/GL/Rectangle.hpp>
#include <Utils/CFG.hpp>
#include <Utils/Utils.hpp>
#include <SOIL.h>

#include <fstream>

std::map<std::string, Texture::TexPair> Texture::textures;
std::map<unsigned int, GLuint> Texture::activeTextures;
GLuint Texture::activeTexture = 0;
CFG* Texture::cfg;

Texture::TexPair::TexPair() {
  textureID = 0;
  sampler = 0;
  count = 0;
  size = vec2(0,0);
}

Texture::Texture() {
  mode = 0;
  filename = "!";
}

Texture::Texture(std::string filename, int mode) {
  this->filename = filename;

  if(textures.count(filename)) {
    textures[filename].count += 1;
    return;
  }
  if(mode < MAP) {
    imageExists(filename);
    loadTexture(filename, (mode == GUI));
  }
  else if(mode == MAP)
    loadMap(filename);
}

Texture::Texture(const Texture& tex) {
  filename = tex.filename;
  VBO = tex.VBO;
  IBO = tex.IBO;
  mode = tex.mode;
  if(textures.count(filename)) {
    textures[filename].count += 1;
  }
}

Texture& Texture::operator= (const Texture& tex) {
  filename = tex.filename;
  VBO = tex.VBO;
  IBO = tex.IBO;
  if(textures.count(filename)) {
    textures[filename].count += 1;
  }
  return *this;
}

Texture::~Texture() {
  if(textures.count(filename)) {
    textures[filename].count -= 1;
    if(textures[filename].count <= 0) {
      glDeleteTextures(1, &textures[filename].textureID);
      glDeleteSamplers(1, &textures[filename].sampler);
      textures.erase(filename);
    }
  }
}

vec2 Texture::getSize() const {
  if(textures.count(filename) > 0)
    return textures[filename].size;
  return vec2(0, 0);
}

bool Texture::isActive(unsigned int textureUnit) const {
  if(activeTextures[textureUnit] == 0) return false;
  return (activeTextures[textureUnit] == textures[filename].textureID);
}

void Texture::bind(unsigned int textureUnit) {
  if(isActive(textureUnit)) return;

  activeTextures[textureUnit] = textures[filename].textureID;
  glActiveTexture(GL_TEXTURE0+textureUnit);
  glBindTexture(GL_TEXTURE_2D, textures[filename].textureID);
  glBindSampler(textureUnit, textures[filename].sampler);
}

void Texture::unbind(unsigned int textureUnit) {
  if(activeTextures[textureUnit] == 0) return;

  activeTextures[textureUnit] = 0;
  glActiveTexture(GL_TEXTURE0+textureUnit);
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindSampler(textureUnit, 0);
}

void Texture::bindCube(unsigned int textureUnit) {
  if(isActive(textureUnit)) return;
  activeTextures[textureUnit] = textures[filename].textureID;
  glActiveTexture(GL_TEXTURE0+textureUnit);
  glBindTexture(GL_TEXTURE_CUBE_MAP, textures[filename].textureID);
  glBindSampler(textureUnit, textures[filename].sampler);
}

vec4 Texture::getPixelColors(int x, int y) {
  if(mode != MAP && mode != EMPTY)
    return vec4(0, 0, 0, 0);

  if(textures[filename].imageMap.count(y) &&
     textures[filename].imageMap[y].count(x)) {
    return textures[filename].imageMap[y][x];
  }
  return vec4(0, 0, 0, 0);
}

GLuint Texture::getGLTexture() {
  if(filename == "!") return 0;
  if(textures.count(filename) == 0) return 0;
  return textures[filename].textureID;
}

GLuint Texture::generateGLTexture(std::string filename) {
  if(textures.count(filename)) return 0;
  textures[filename];
  glGenTextures(1, &textures[filename].textureID);
  glGenSamplers(1, &textures[filename].sampler);
  return textures[filename].textureID;
}

/**
 * @brief
 *   Recalculates the geometry of the rectangle associated
 *   with the texture by changing it to the given one.
 *
 * @param rect
 */
void Texture::recalculateGeometry(const Rect& rect) {
  this->rect.change(rect, false);
}

/**
 * @brief
 *   Draws the texture by binding it to slot 0 followed
 *   by drawing a rectangle, which effectively draws the texture
 *   onto a rectangle
 */
void Texture::draw() {
  bind(0);
  rect.draw();
}

bool Texture::loadTexture(std::string filename, bool isGUI) {
  if(textures.count(filename)) return true;

  textures[filename];
  mode = (isGUI) ? GUI : TEXTURE;
  glGenTextures(1, &textures[filename].textureID);
  glGenSamplers(1, &textures[filename].sampler);
  bind(0);

  if(filename.substr(filename.size()-4) == ".dds")
    loadDDS();
  else
    loadTexture();

  textures[filename].count += 1;
  return true;
}

bool Texture::loadTexture() {
  int width, height;
  unsigned char* image;

  image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGBA);

  if(image == 0) throw Error("Failed to load SOIL_IMAGE");

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

  if(!Utils::getGLError()) throw Error("Failed to load SOIL_IMAGE");

  if(mode == TEXTURE) {
    generateMipmaps();
    linearMipmap();
    //setSamplerFiltering(GL_TEXTURE_MAX_ANISOTROPY_EXT, cfg->graphics.aniso);
  }
  if(mode == GUI) {
    linear();
    clampToEdge();
  }
  SOIL_free_image_data(image);
  textures[filename].size = vec2(width, height);
  return true;
}

bool Texture::loadDDS() {
  mode = DDS;
  textures[filename].textureID = SOIL_load_OGL_texture(
    filename.c_str(),
    SOIL_LOAD_AUTO,
    textures[filename].textureID,
    SOIL_FLAG_MIPMAPS | SOIL_FLAG_COMPRESS_TO_DXT
  );
  if(textures[filename].textureID == 0 || !Utils::getGLError())
    throw Error("Failed to load DDS image");
  linearMipmap();
  //if(cfg->graphics.aniso >= 1)
  //  setSamplerFiltering(GL_TEXTURE_MAX_ANISOTROPY_EXT, cfg->graphics.aniso);
  textures[filename].size = vec2();
  return true;
}

bool Texture::loadCubeTexture(const std::vector<std::string>& filenames) {
  if(filenames.size() != 6) return false;
  unsigned char *image;
  int width, height;
  std::vector<GLenum> types = {
    GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
  };
  generateGLTexture(filenames[0]);
  bindCube(0);
  for(unsigned int i = 0; i < filenames.size(); i++) {
    image = SOIL_load_image(filenames[0].c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
    if(image == 0)
      throw Error("Failed to load cubemap image.");
    glTexImage2D(types[i], 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    SOIL_free_image_data(image);
  }
  linear();
  clampToEdge();
  setSamplerFiltering(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  generateMipmaps(GL_TEXTURE_CUBE_MAP);
  textures[filename].count += 1;
  textures[filename].size = vec2(width, height);
  return true;
}

bool Texture::loadMap(std::string filename) {
  this->filename = filename;
  textures[filename];
  mode = MAP;
  int width;
  int height;
  unsigned char* image;
  image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
  if(image == 0) {
    SOIL_free_image_data(image);
    return false;
  }
  if(!Utils::getGLError())
    throw Error("Failed to load SOIL_IMAGE");
  int processed = 0;
  for(int y = 0; y < height; y++) {
    for(int x = 0; x < width; x++) {
      textures[filename].imageMap[y][x] = vec4(
        image[processed], image[processed+1],
        image[processed+2], image[processed+3]
      );
      processed += 4;
    }
  }
  textures[filename].size = vec2(width, height);
  SOIL_free_image_data(image);
  return true;
}

unsigned char* Texture::loadEmptyTexture(const vec2& size, bool isReturn) {
  const int arraySize = size.x*size.y;
  unsigned char *texData = new unsigned char[arraySize];
  for(int i = 0; i < arraySize; i++)
    texData[i] = 255;

  createTexture(size, texData, GL_RED);
  generateMipmaps();
  linear();

  textures[filename].size = size;
  textures[filename].count += 1;
  if(isReturn)
    return texData;

  delete[] texData;
  return NULL;
}

bool Texture::createTexture(const vec2& s, unsigned char* data, GLenum color, GLenum inColor) {
  static unsigned int nameNumber = 0;
  nameNumber++;
  filename = "createTexture" + Utils::toStr(nameNumber);
  textures[filename];
  mode = EMPTY;
  inColor = (color == GL_DEPTH_COMPONENT) ? GL_DEPTH_COMPONENT : inColor;

  glGenTextures(1, &textures[filename].textureID);
  glGenSamplers(1, &textures[filename].sampler);
  bind(0);

  glTexImage2D(GL_TEXTURE_2D, 0, color, s.x, s.y, 0, inColor, GL_UNSIGNED_BYTE, data);
  if(!Utils::getGLError())
    throw Error("createTexture failed. View OpenGL error above");
  textures[filename].size = s;
  textures[filename].count += 1;
  return true;
}

void Texture::changeSubTex(const vec2& pos, const vec2& size, unsigned char* change) {
  bind(0);
  glTexSubImage2D(
    GL_TEXTURE_2D,
    0,
    pos.x, pos.y,
    size.x, size.y,
    GL_RED, GL_UNSIGNED_BYTE,
    change
  );
}

void Texture::saveTexture(std::string filename, const vec2& size) {
  const long imageSize = size.x*size.y*3;
  unsigned char* data = new unsigned char[imageSize];
  bind(0);
  glGetTexImage(GL_TEXTURE_2D, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
  int xa = (int)size.x % 256;
  int xb = (size.x-xa)/256;
  int ya = (int)size.y % 256;
  int yb = (size.y-ya)/256;
  unsigned char header[18] = {
    0,0,2,0,0,0,0,0,0,0,0,0,
    (unsigned char)xa, (unsigned char)xb,
    (unsigned char)ya, (unsigned char)yb,
    24, 0
  };

  for(unsigned int i = 0; i < imageSize; i+= 3) {
    data[i+1] = data[i+1]+25;
    data[i+2]   = data[i+2]+25;
    data[i] = data[i]+25;
  }

  std::fstream f(filename, std::ios::out | std::ios::binary);
  f.write(reinterpret_cast<char*>(header), sizeof(char)*18);
  f.write(reinterpret_cast<char*>(data), sizeof(char)*imageSize);
  f.close();
  delete[] data;
  data = NULL;
  log("Saved texture to: '", filename, "'");
}

void Texture::freeImageMap() {
  if(mode != MAP || !textures.count(filename))
    return;
  textures[filename].imageMap.clear();
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
  if(textures.count(filename) == 0 || textures[filename].sampler == 0) return;
  glSamplerParameteri(textures[filename].sampler, param, value);
  if(!Utils::getGLError())
    throw Error("Failed to set Sampler Parameteri");
}

Texture& Texture::generateMipmaps(GLenum target) {
  if (!isActive(0) && target != GL_TEXTURE_CUBE_MAP)
    bind(0);

  glGenerateMipmap(target);
  if(!Utils::getGLError())
    throw Error("Failed to generateMipmaps");

  return *this;
}

void Texture::imageExists(std::string filename) {
  std::ifstream fs(filename);
  bool exists = false;
  exists = fs.is_open();
  std::string errorMessage = "Cannot open image: " + filename;
  if(!exists)
    throw Error(errorMessage.c_str());
  else
    fs.close();
}

void Texture::unbindAll() {
  for(auto k : activeTextures) {
    if(k.second != 0) {
      glActiveTexture(GL_TEXTURE0+k.first);
      glBindTexture(GL_TEXTURE_2D, 0);
      glBindSampler(0, 0);
      k.second = 0;
    }
  }
}

void Texture::init(CFG* c) {
  cfg = c;
  /* GLfloat maxAniso = 0.0f; */
  //glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
  if(!Utils::getGLError())
    throw Error(":D");
  //if(maxAniso < cfg->graphics.aniso)
  //  cfg->graphics.aniso = maxAniso;
  for(unsigned int i = 0; i < 10; i++)
    activeTextures[i] = 0;
}



