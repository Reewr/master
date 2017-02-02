#include "Shader.hpp"

#include <fstream>
#include <vector>

#include "../Utils/Utils.hpp"

std::string loadTextfile(const std::string& filename) {
  std::ifstream fs(filename);
  std::string   content;
  std::string   line;

  if (!fs.is_open()) {
    throw std::runtime_error("Unable to open file: " + filename);
  }

  while (fs.good()) {
    std::getline(fs, line);
    bool hasInclude = line.find("#include") != std::string::npos;

    if (hasInclude) {
      int first  = line.find("<");
      int length = line.find(">") - first - 1;
      content += loadTextfile(line.substr(first, length));
    } else
      content += line + '\n';
  }

  fs.close();

  return content;
}

Shader::Shader() : mId(0), mFilename("Unknown") {}

Shader::Shader(const std::string& filename) {
  mId       = loadShader(filename);
  mFilename = filename;
}

Shader::Shader(const std::string& src,
               bool               isFragment,
               const std::string& fname) {
  mId       = fromSource(src, isFragment, fname);
  mFilename = fname;
}

Shader::~Shader() {
  if (mId == 0)
    return;

  glDeleteShader(mId);
}

std::string Shader::filename() const {
  return mFilename;
}

std::string Shader::type() const {
  return mType;
}

GLuint Shader::id() const {
  return mId;
}

GLuint Shader::fromSource(const std::string& src,
                          bool               isFragment,
                          const std::string& fname) {
  GLuint      shaderType = (isFragment) ? GL_FRAGMENT_SHADER : GL_VERTEX_SHADER;
  GLuint      shaderID   = glCreateShader(shaderType);
  std::string shaderSrc  = src;
  const char* source     = shaderSrc.c_str();

  glShaderSource(shaderID, 1, &source, 0);
  glCompileShader(shaderID);

  shaderID = (checkShader(shaderID, fname)) ? shaderID : 0;
  return shaderID;
}

GLuint Shader::loadShader(const std::string& filename) {
  GLuint shaderType = GL_VERTEX_SHADER;

  if (filename.find(".fs") != std::string::npos)
    shaderType = GL_FRAGMENT_SHADER;

  GLuint      shaderID      = glCreateShader(shaderType);
  std::string shaderCodeStr = loadTextfile(filename);
  const char* source        = shaderCodeStr.c_str();

  glShaderSource(shaderID, 1, &source, 0);
  glCompileShader(shaderID);

  shaderID = (checkShader(shaderID, filename)) ? shaderID : 0;

  return shaderID;
}

bool Shader::checkShader(const GLuint id, const std::string& filename) {
  GLint isCompiled = 0;
  GLint maxLength  = 0;

  // retrieve the shader information
  glGetShaderiv(id, GL_COMPILE_STATUS, &isCompiled);
  glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);

  // throw error if it cant compile
  if (isCompiled == GL_FALSE) {
    std::vector<GLchar> infoLog(maxLength);
    std::string s = filename + ": ";

    glGetShaderInfoLog(id, maxLength, &maxLength, &infoLog[0]);
    glDeleteShader(id);

    log("Errors in shader: ", filename, " - see below:");
    for (unsigned int i = 0; i < infoLog.size(); i++) {
      if (infoLog[i] == '\n') {
        log(s);
        s = filename + ": ";
      } else
        s += infoLog[i];
    }
    log("");

    throw std::runtime_error("Shader.cpp");
  }

  // if there some information
  if (maxLength > 1) {
    std::vector<GLchar> infoLog(maxLength);
    std::string s = "";

    glGetShaderInfoLog(id, maxLength, &maxLength, &infoLog[0]);

    for (unsigned int i = 0; i < infoLog.size(); i++)
      s += infoLog[i];

    warning(s.c_str(), filename.c_str());
  }

  return true;
}