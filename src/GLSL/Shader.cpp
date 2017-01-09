#include "Shader.hpp"

#include <vector>
#include <fstream>

#include "../Utils/Utils.hpp"

std::string loadTextfile(const std::string& filename) {
  std::ifstream fs(filename);
  std::string content;
  std::string line;

  if (!fs.is_open()) {
    throw new Error("Unable to open file: " + filename);
  }

  while (fs.good()) {
    std::getline(fs, line);
    bool hasInclude = line.find("#include") != std::string::npos;

    if (hasInclude) {
      int first = line.find("<");
      int length = line.find(">")-first-1;
      content += loadTextfile(line.substr(first,length));
    }
    else
      content += line + '\n';
  }

  fs.close();

  return content;
}

Shader::Shader() {
  id = 0;
  filename = "Unknown";
}

Shader::Shader(const std::string& filename) {
  id = loadShader(filename);
  this->filename = filename;
}

Shader::Shader(const std::string& src, bool isFragment, const std::string& fname) {
  id = fromSource(src, isFragment, fname);
  this->filename = fname;
}

Shader::~Shader() {
  if(id == 0) return;
  glDeleteShader(id);
}

GLuint Shader::fromSource(const std::string& src, bool isFragment, const std::string& fname) {
  GLuint shaderType = (isFragment) ? GL_FRAGMENT_SHADER : GL_VERTEX_SHADER;
  GLuint shaderID = glCreateShader(shaderType);
  std::string shaderSrc = src;
  const char* source = shaderSrc.c_str();

  glShaderSource(shaderID, 1, &source, 0);
  glCompileShader(shaderID);

  shaderID = (checkShader(shaderID, fname)) ? shaderID : 0;
  return shaderID;
}

GLuint Shader::loadShader(const std::string& filename) {
  GLuint shaderType = GL_VERTEX_SHADER;

  if(filename.find(".fs") != std::string::npos)
    shaderType = GL_FRAGMENT_SHADER;

  GLuint shaderID = glCreateShader(shaderType);
  std::string shaderCodeStr = loadTextfile(filename);
  const char *source = shaderCodeStr.c_str();

  glShaderSource(shaderID, 1, &source, 0);
  glCompileShader(shaderID);

  shaderID = (checkShader(shaderID, filename)) ? shaderID : 0;

  return shaderID;
}

bool Shader::checkShader(const GLuint id, const std::string& filename) {
  GLint isCompiled = 0;
  GLint maxLength = 0;
  glGetShaderiv(id, GL_COMPILE_STATUS, &isCompiled);
  glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);
  if(isCompiled == GL_FALSE) {
    std::vector<GLchar> infoLog(maxLength);
    glGetShaderInfoLog(id, maxLength, &maxLength, &infoLog[0]);
    glDeleteShader(id);
    std::string s = filename + ": ";
    log("Errors in shader: ", filename, " - see below:");
    for(unsigned int i = 0; i < infoLog.size(); i++) {
      if(infoLog[i] == '\n') {
        log(s);
        s = filename+": ";
      }
      else
        s += infoLog[i];

    }
    log("");
    throw Error("Shader.cpp");
    return false;
  }
  else if(maxLength > 1) {
    std::vector<GLchar> infoLog(maxLength);
    glGetShaderInfoLog(id, maxLength, &maxLength, &infoLog[0]);
    std::string s = "";
    for(unsigned int i = 0; i < infoLog.size(); i++)
      s += infoLog[i];
    warning(s.c_str(), filename.c_str());
  }

  return true;
}