#include "Program.hpp"

#include <fstream>
#include <iostream>

#include "../GlobalLog.hpp"
#include "../Utils/Utils.hpp"
#include "../Utils/str.hpp"
#include "Shader.hpp"

GLuint Program::activeProgram = 0;

Program::Program()
    : Logging::Log("Program"), program(0), isLinked(false), isUsable(false) {}

Program::Program(const std::string& fsvs, bool link)
    : Logging::Log("Program"), program(0), isLinked(false), isUsable(false) {
  createProgram(fsvs, link);
}

Program::~Program() {
  if (mShaders.size() > 0) {
    for (auto& s : mShaders)
      delete s.second;

    mShaders.clear();
  }
}


bool Program::createProgram(const std::string& shaders, bool link) {
  if (shaders.find(",") == std::string::npos) {
    throw std::runtime_error(
      "Must load program as single string of two shaders");
  }

  std::map<Shader::Type, std::string> srcs =
    loadMultipleShaderFilename(shaders);

  if (mShaders.size() > 0) {
    for (auto& s : mShaders)
      delete s.second;

    mShaders.clear();
  }

  for (auto& s : srcs) {
    mShaders[s.first] = new Shader(s.second);

    if (mShaders[s.first]->id() == 0) {
      throw std::runtime_error("Failed to create program due to shader error");
    }
  }

  if (program != 0)
    glDeleteProgram(program);

  program = glCreateProgram();

  if (program == 0)
    throw std::runtime_error("Failed to create program");

  for (auto& s : mShaders) {
    if (!addShader(*s.second)) {
      throw std::runtime_error("Failed to create program due to " +
                               Shader::typeToStr(s.first));
    }
  }

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
  checkErrors("addShader()");
  return true;
}

bool Program::link() {
  if (isLinked)
    return true;

  glLinkProgram(program);

  isLinked = true;
  isUsable = checkProgram(program);
  checkErrors("link()");

  // Set the binding layouts if the shader has that.
  for (auto& s : mShaders) {
    const Shader::Details& details = s.second->details();

    for (auto& binding : details.layoutBindings) {
      mLog->debug("Binding layouts for: {}, {} to {}",
                  s.second->filename(),
                  binding.name,
                  binding.location);
      setUniform(binding.name, binding.location);
    }
  }

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
    mLog->warn("{} - {} does not exist in shader", mFilename, uni);
    return loc;
  }

  checkErrors("getUniformLocation(): " + uni);
  return loc;
}

GLint Program::getAttribLocation(const std::string& attrib) {
  if (program == 0 || !isUsable)
    return -1;

  GLint loc = glGetAttribLocation(program, attrib.c_str());
  checkErrors("getAttribLocation(): " + attrib);
  return loc;
}

bool Program::setGLUniform(GLint loc, const bool b) {
  glUniform1i(loc, (int) b);
  return checkErrors("setGLUniform(bool): ");
}

bool Program::setGLUniform(GLint loc, const int i) {
  glUniform1i(loc, i);
  return checkErrors("setGLUniform(int): ");
}

bool Program::setGLUniform(GLint loc, const int i, const int j) {
  glUniform2i(loc, i, j);
  return checkErrors("setGLUniform(int, int): ");
}

bool Program::setGLUniform(GLint loc, const float f) {
  glUniform1f(loc, f);
  return checkErrors("setGLUniform(float): ");
}

bool Program::setGLUniform(GLint loc, const double d) {
  glUniform1f(loc, d);
  return checkErrors("setGLUniform(double): ");
}

bool Program::setGLUniform(GLint loc, const mmm::vec2& v) {
  glUniform2f(loc, v.x, v.y);
  return checkErrors("setGLUniform(vec2): ");
}

bool Program::setGLUniform(GLint loc, const mmm::vec3& v) {
  glUniform3f(loc, v.x, v.y, v.z);
  return checkErrors("setGLUniform(vec3): ");
}

bool Program::setGLUniform(GLint loc, const mmm::vec4& v) {
  glUniform4f(loc, v.x, v.y, v.z, v.w);
  return checkErrors("setGLUniform(vec4): ");
}

bool Program::setGLUniform(GLint loc, const mmm::mat3& m) {
  glUniformMatrix3fv(loc, 1, GL_TRUE, m.rawdata);
  return checkErrors("setGLUniform(mat3): ");
}

bool Program::setGLUniform(GLint loc, const mmm::mat4& m) {
  glUniformMatrix4fv(loc, 1, GL_TRUE, m.rawdata);
  return checkErrors("setGLUniform(mat4): ");
}

bool Program::bindAttrib(const std::string& attrib, const int index) {
  if (isLinked)
    return false;

  glBindAttribLocation(program, (const GLuint) index, attrib.c_str());
  checkErrors("bindAttrib(): " + attrib);

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

std::map<Shader::Type, std::string>
Program::loadMultipleShaderFilename(const std::string& files) {
  std::map<Shader::Type, std::string> filenames;
  std::vector<std::string> shaders = str::split(files, ',');

  for (auto& s : shaders) {
    filenames[Shader::typeFromFilename(s)] = s;
  }

  if (filenames.count(Shader::Type::Fragment) == 0)
    throw std::runtime_error("Multiple shaders filename '" + files +
                             "' is missing fragment shader");

  if (filenames.count(Shader::Type::Vertex) == 0)
    throw std::runtime_error("Multiple shaders filename '" + files +
                             "' is missing vertex shader");

  return filenames;
}

bool Program::checkErrors(const std::string& place) {
  GLenum errorCheck = glGetError();

  if (errorCheck == GL_NO_ERROR) {
    return true;
  }

  std::string message = "Error in these shaders: ";
  int         size    = mShaders.size();
  int         index   = 0;
  for (auto& a : mShaders) {
    message += a.second->filename() + (index + 1 == size ? "" : ", ");
  }

  mLog->error(message);
  mLog->error("OpenGL Error Code: {}", errorCheck);

  return false;
}

bool Program::checkProgram(const GLuint pro) {
  GLint isLinked = GL_FALSE;
  glGetProgramiv(pro, GL_LINK_STATUS, (int*) &isLinked);

  if (isLinked == GL_TRUE)
    return true;

  GLint maxLen = 0;
  glGetProgramiv(pro, GL_INFO_LOG_LENGTH, &maxLen);

  std::vector<GLchar> infoLog(maxLen);
  glGetProgramInfoLog(pro, maxLen, &maxLen, &infoLog[0]);

  mLog->error("Error in GLSL Program");
  mLog->error(std::string(infoLog.begin(), infoLog.end()));

  glDeleteProgram(pro);

  return false;
}
