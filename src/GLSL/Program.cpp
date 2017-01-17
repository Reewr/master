#include "Program.hpp"

#include <fstream>

#include "Shader.hpp"

GLuint Program::activeProgram = 0;

Program::Program() {
  isLinked = false;
  isUsable = false;
  program  = 0;
}

Program::Program(const Shader& frag, const Shader& vertex, const bool link) {
  isLinked = false;
  isUsable = false;
  program  = 0;
  createProgram(frag, vertex, link);
}

Program::Program(const std::string& fs, const Shader& vertex, const bool link) {
  isLinked = false;
  isUsable = false;
  program  = 0;
  createProgram(fs, vertex, link);
}

Program::Program(const Shader& frag, const std::string& vs, const bool link) {
  isLinked = false;
  isUsable = false;
  program  = 0;
  createProgram(frag, vs, link);
}

Program::Program(const std::string& fs,
                 const std::string& vs,
                 const bool         link) {
  isLinked = false;
  isUsable = false;
  program  = 0;
  createProgram(fs, vs, link);
}

Program::Program(const std::string& fsvs, int link) {
  isLinked = false;
  isUsable = false;
  program  = 0;
  createProgram(fsvs, link);
}

Program::~Program() {
  if (program != 0)
    glDeleteProgram(program);
  program = 0;
  filenames.clear();
  uniLocations.clear();
}

bool Program::createProgram(const std::string& fs,
                            const std::string& vs,
                            const bool         link) {
  Shader frag(fs);
  Shader vertex(vs);
  return createProgram(frag, vertex, link);
}

bool Program::createProgram(const std::string& fs,
                            const Shader&      vertex,
                            const bool         link) {
  Shader frag(fs);
  return createProgram(frag, vertex, link);
}

bool Program::createProgram(const Shader&      frag,
                            const std::string& vs,
                            const bool         link) {
  Shader vertex(vs);
  return createProgram(frag, vertex, link);
}

bool Program::createProgram(const Shader& frag,
                            const Shader& vertex,
                            const bool    link) {
  isUsable = false;
  isLinked = false;
  if (program != 0)
    glDeleteProgram(program);
  program = 0;
  filenames.push_back(frag.filename());
  filenames.push_back(vertex.filename());

  program = glCreateProgram();

  if (program == 0 || frag.id() == 0 || vertex.id() == 0)
    throw Error("Failed to create program");

  if (!addShader(frag) || !addShader(vertex))
    throw Error("Failed to create program");

  if (!link)
    return true;
  return this->link();
}

bool Program::createProgram(const std::string& fsvs, int link) {
  std::map<std::string, std::string> srcs = loadVSFS(fsvs);
  if (!srcs.count("FRAGMENT") || !srcs.count("VERTEX"))
    throw Error("Failed to load one of the shaders in combined file.");
  Shader fs(srcs["FRAGMENT"], true, fsvs);
  Shader vs(srcs["VERTEX"], false, fsvs);
  bool   linkage = (link == 0);
  return createProgram(fs, vs, linkage);
}

void Program::deleteProgram() {
  if (program != 0) {
    glDeleteProgram(program);
    isLinked = false;
    isUsable = false;
    program  = 0;
  }
}

bool Program::addShader(const Shader& sh) {
  if (program == 0) {
    program = glCreateProgram();
  }
  glAttachShader(program, sh.id());
  checkErrors("addShader()", { sh.type() });
  return true;
}

bool Program::addShader(const std::string& sh) {
  Shader shader(sh);
  return addShader(shader);
}

bool Program::link() {
  if (isLinked)
    return false;
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
    throw Error("Tried to bind program that is not usable.");
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
    tlog(filenames[0],
         ", ",
         filenames[1],
         " - " + uni + " does not exist in shader.");
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

bool Program::setGLUniform(GLint loc, const vec2& v) {
  glUniform2f(loc, v.x, v.y);
  return checkErrors("setGLUniform(vec2): ", filenames);
}

bool Program::setGLUniform(GLint loc, const vec3& v) {
  glUniform3f(loc, v.x, v.y, v.z);
  return checkErrors("setGLUniform(vec3): ", filenames);
}

bool Program::setGLUniform(GLint loc, const vec4& v) {
  glUniform4f(loc, v.x, v.y, v.z, v.w);
  return checkErrors("setGLUniform(vec4): ", filenames);
}

bool Program::setGLUniform(GLint loc, const mat3& m) {
  glUniformMatrix3fv(loc, 1, GL_TRUE, m.rawdata);
  return checkErrors("setGLUniform(mat3): ", filenames);
}

bool Program::setGLUniform(GLint loc, const mat4& m) {
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

std::string Program::loadShader(std::ifstream& f) {
  if (!f.is_open())
    throw Error("ifstream is closed.");
  std::string shaderSrc = "";
  std::string line;
  while (f.good()) {
    std::getline(f, line);
    if (line.find("#endif") != std::string::npos)
      return shaderSrc;
    shaderSrc += '\n';
    shaderSrc += line;
  }
  shaderSrc += '\0';
  return shaderSrc;
}

std::map<std::string, std::string> Program::loadVSFS(const std::string& fsvs) {
  if (!fsvs.find(".vsfs") && !fsvs.find(".fsvs"))
    throw Error("File extension is faulty.");

  std::ifstream fs(fsvs);
  std::map<std::string, std::string> source;
  std::string content;
  std::string line;
  if (!fs.is_open())
    throw Error("Unable to open shaderfile.");
  while (fs.good()) {
    std::getline(fs, line);
    if (line.find("#ifdef") != std::string::npos) {
      int first  = line.find("__") + 2;
      int second = line.find("__", first + 1) - first;
      content    = loadShader(fs);
      source[line.substr(first, second)] = content;
    }
  }
  fs.close();
  return source;
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
    throw Error("Program.cpp");
    return false;
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
    throw Error("Program.cpp");
    return false;
  }
  return true;
}
