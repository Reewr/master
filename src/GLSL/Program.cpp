#include "Program.hpp"

#include <fstream>
#include <iostream>

#include "../GlobalLog.hpp"
#include "../Utils/Utils.hpp"
#include "Shader.hpp"

GLuint Program::activeProgram = 0;

Program::Program()
    : Logging::Log("Program"), program(0), isLinked(false), isUsable(false) {}

Program::~Program() {}

Program::Program(const std::string& fsvs, bool link)
    : Logging::Log("Program"), program(0), isLinked(false), isUsable(false) {
  createProgram(fsvs, link);
}

bool Program::createProgram(const std::string& fsvs, bool link) {
  if (fsvs.find(",") == std::string::npos) {
    throw std::runtime_error(
      "Must load program as single string of two shaders");
  }

  std::map<std::string, std::string> srcs = loadDualShaderFilename(fsvs);

  Shader fs(srcs["FRAGMENT"]);
  Shader vs(srcs["VERTEX"]);

  if (program != 0)
    glDeleteProgram(program);

  program = 0;
  filenames.push_back(fs.filename());
  filenames.push_back(vs.filename());

  program = glCreateProgram();

  if (program == 0 || fs.id() == 0 || vs.id() == 0)
    throw std::runtime_error("Failed to create program");

  if (!addShader(fs))
    throw std::runtime_error("Failed to create program due to fragment shader");

  if (!addShader(vs))
    throw std::runtime_error("Failed to create program due to vertex shader");

  if (!link)
    return true;

  return this->link();
}

bool Program::load(ResourceManager*) {
  mLog->debug("Loading: {}", mFilename);
  return createProgram(mFilename, true);
}

void Program::unload() {
  mLog->debug("Unloading: {}", mFilename);
  if (program != 0) {
    if (activeProgram == program)
      activeProgram = 0;

    glDeleteProgram(program);
    program = 0;
  }

  isUsable = false;
  isLinked = false;
}

bool Program::addShader(const Shader& sh) {
  if (program == 0) {
    program = glCreateProgram();
  }

  glAttachShader(program, sh.id());
  checkErrors("addShader()", { sh.type() });
  return true;
}

bool Program::link() {
  if (isLinked)
    return true;

  glLinkProgram(program);

  isLinked = true;
  isUsable = checkProgram(program);
  checkErrors("link()", filenames);
  return isUsable;
}

void Program::bind() {
  if (isActive())
    return;

  if (!isUsable)
    throw std::runtime_error("Tried to bind program that is not usable.");

  activeProgram = program;
  glUseProgram(program);
}

GLint Program::getUniformLocation(const std::string& uni) {
  if (program == 0 || !isUsable)
    return -1;
  if (uniLocations.count(uni) > 0)
    return uniLocations[uni];

  GLint loc = glGetUniformLocation(program, uni.c_str());
  if (loc != -1)
    uniLocations[uni] = loc;
  else {
    mLog->debug("{}, {} - {} does not exist in shader",
                filenames[0],
                filenames[1],
                uni);
    return loc;
  }
  checkErrors("getUniformLocation(): " + uni, filenames);
  return loc;
}

GLint Program::getAttribLocation(const std::string& attrib) {
  if (program == 0 || !isUsable)
    return -1;

  GLint loc = glGetAttribLocation(program, attrib.c_str());
  checkErrors("getAttribLocation(): " + attrib, filenames);
  return loc;
}

bool Program::setGLUniform(GLint loc, const bool b) {
  glUniform1i(loc, (int) b);
  return checkErrors("setGLUniform(bool): ", filenames);
}

bool Program::setGLUniform(GLint loc, const int i) {
  glUniform1i(loc, i);
  return checkErrors("setGLUniform(int): ", filenames);
}

bool Program::setGLUniform(GLint loc, const int i, const int j) {
  glUniform2i(loc, i, j);
  return checkErrors("setGLUniform(int, int): ", filenames);
}

bool Program::setGLUniform(GLint loc, const float f) {
  glUniform1f(loc, f);
  return checkErrors("setGLUniform(float): ", filenames);
}

bool Program::setGLUniform(GLint loc, const double d) {
  glUniform1f(loc, d);
  return checkErrors("setGLUniform(double): ", filenames);
}

bool Program::setGLUniform(GLint loc, const mmm::vec2& v) {
  glUniform2f(loc, v.x, v.y);
  return checkErrors("setGLUniform(vec2): ", filenames);
}

bool Program::setGLUniform(GLint loc, const mmm::vec3& v) {
  glUniform3f(loc, v.x, v.y, v.z);
  return checkErrors("setGLUniform(vec3): ", filenames);
}

bool Program::setGLUniform(GLint loc, const mmm::vec4& v) {
  glUniform4f(loc, v.x, v.y, v.z, v.w);
  return checkErrors("setGLUniform(vec4): ", filenames);
}

bool Program::setGLUniform(GLint loc, const mmm::mat3& m) {
  glUniformMatrix3fv(loc, 1, GL_TRUE, m.rawdata);
  return checkErrors("setGLUniform(mat3): ", filenames);
}

bool Program::setGLUniform(GLint loc, const mmm::mat4& m) {
  glUniformMatrix4fv(loc, 1, GL_TRUE, m.rawdata);
  return checkErrors("setGLUniform(mat4): ", filenames);
}

bool Program::bindAttrib(const std::string& attrib, const int index) {
  if (isLinked)
    return false;
  glBindAttribLocation(program, (const GLuint) index, attrib.c_str());
  checkErrors("bindAttrib(): " + attrib, filenames);
  return true;
}

bool Program::bindAttribs(const std::vector<std::string>& attribs,
                          const std::vector<int>&         indicies) {
  if (attribs.size() != indicies.size())
    return false;
  for (unsigned int i = 0; i < attribs.size(); i++) {
    if (!bindAttrib(attribs[i], indicies[i]))
      return false;
  }
  return true;
}

bool Program::isActive() const {
  return (activeProgram == program);
}

std::map<std::string, std::string>
Program::loadDualShaderFilename(const std::string& fsvs) {
  size_t      commaPos = fsvs.find(",");
  std::string f1       = fsvs.substr(0, commaPos);
  std::string f2       = fsvs.substr(commaPos + 1);

  bool f1isVert = f1.find(".vs") != std::string::npos;
  bool f1isFrag = f1.find(".fs") != std::string::npos;

  bool f2isVert = f2.find(".vs") != std::string::npos;
  bool f2isFrag = f2.find(".fs") != std::string::npos;

  if (!f1isFrag && !f2isFrag)
    throw std::runtime_error("Dual filename '" + fsvs +
                             "' is missing fragment shader");

  if (!f1isVert && !f2isVert)
    throw std::runtime_error("Dual filename '" + fsvs +
                             "' is missing vertex shader");

  if (f1isFrag && f2isFrag)
    throw std::runtime_error("Dual filename '" + fsvs +
                             "' has two fragment shaders");

  if (f1isVert && f2isVert)
    throw std::runtime_error("Dual filename '" + fsvs +
                             "' has two vertex shaders");

  return {
    { "FRAGMENT", f1isFrag ? f1 : f2 }, { "VERTEX", f1isVert ? f1 : f2 },
  };
}

bool Program::checkErrors(const std::string&              place,
                          const std::vector<std::string>& filenames) {
  GLenum errorCheck = glGetError();
  if (errorCheck != GL_NO_ERROR) {
    std::cout << "Error below in these: ";
    for (std::string s : filenames)
      std::cout << s + ", ";
    std::cout << std::endl;
    fprintf(stderr, "GL_ERROR @ %s: %i\n", place.c_str(), errorCheck);
    std::cout << std::endl;
    error("GLSL error.. Please see error above.");
    throw std::runtime_error("Program.cpp");
  }

  return true;
}

bool Program::checkProgram(const GLuint pro) {
  GLint isLinked = GL_FALSE;
  glGetProgramiv(pro, GL_LINK_STATUS, (int*) &isLinked);
  if (isLinked == GL_FALSE) {
    GLint maxLength = 0;
    glGetProgramiv(pro, GL_INFO_LOG_LENGTH, &maxLength);

    std::vector<GLchar> infoLog(maxLength);
    glGetProgramInfoLog(pro, maxLength, &maxLength, &infoLog[0]);

    std::string s = "";
    for (unsigned int i = 0; i < infoLog.size(); i++)
      s += infoLog[i];
    error(s);
    glDeleteProgram(pro);
    error("GLSL program error. Please see above.");
    throw std::runtime_error("Program.cpp");
  }

  return true;
}
