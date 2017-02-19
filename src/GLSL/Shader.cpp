#include "Shader.hpp"
#include <fstream>
#include <vector>

#include "../Utils/CFG.hpp"
#include "../Utils/Utils.hpp"

CFG* Shader::mCFG = nullptr;

// this variable contains the seperators that `findFirstSeperator`
// will look for.
const std::string SHADER_CFG_SEPARATORS = ";+*/-, ";

/**
 * @brief
 *   Finds the first of a series of seperators within the line. Unline
 *   `find_first_of`, this actually checks them all and sees which is
 *   first.
 *
 * @param line
 * @param startPos The start pos of where to search.
 *
 * @return
 */
size_t findFirstSeperator(std::string& line, size_t startPos = 0) {
  size_t pos = std::string::npos;

  for (auto a : SHADER_CFG_SEPARATORS) {
    size_t sepPos = line.find_first_of(a, startPos);

    if (sepPos < pos)
      pos = sepPos;
  }

  return pos;
}

/**
 * @brief
 *   If the shader contains references to _CFG_, this function can be used
 *   to parse that expression and replace the referenced variable with an
 *   item in the configuration file.
 *
 *   This can be used to increase the performance of a shader when a variable
 *   from the config file is needed.
 *
 * @param line the line to parse
 * @param cfg a reference to the CFG
 *
 * @return
 */
void replaceCFGExpression(std::string& line) {
  size_t pos = line.find("_CFG_");

  while (pos != std::string::npos) {
    if (pos + 6 > line.size())
      throw new std::runtime_error("Malformed CFG expression");

    size_t      sepPos = findFirstSeperator(line, pos);
    std::string key    = line.substr(pos + 6, sepPos - pos - 6);
    std::string option = Shader::mCFG->getPropAsType(key);

    if (option == "")
      throw std::runtime_error("Unable to find property: '" + key + "' in CFG");

    line.replace(pos, key.size() + 6, option);

    pos = line.find("_CFG_");
  }
}

/**
 * @brief
 *   Loads the shader from file, handling any special syntax as it detects it.
 *   This function supports two special syntaxes:
 *
 *   1. #include
 *   2. _CFG_
 *
 *   The first, `#include`, will let you include a file from file in its
 *   entirety. Please keep in mind that this is simple replace `include`
 *   with whole file it is referencing.
 *
 *   The second, `_CFG_`, lets you reference the CFG by properties. For instance
 *   you can say something like:
 *
 *   `const vec2 screenRes = _CFG_.Graphics.Resolution`
 *
 *   This will replace `_CFG_.Graphics.Resolution` with `vec2(screenX, screenY)
 *   where `screenX` and `screenY` is the resolution set in the configuration
 *   file.
 *
 * @param filename
 *
 * @return
 */
std::string loadTextfile(const std::string& filename) {
  std::ifstream fs(filename);
  std::string   content;
  std::string   line;

  if (!fs.is_open()) {
    throw std::runtime_error("Unable to open file: " + filename);
  }

  while (fs.good()) {
    std::getline(fs, line);
    bool hasInclude       = line.find("#include") != std::string::npos;
    bool hasCFGExpression = line.find("_CFG_") != std::string::npos;

    if (hasInclude) {
      int first  = line.find("<");
      int length = line.find(">") - first - 1;
      content += loadTextfile(line.substr(first, length));
    } else {
      if (hasCFGExpression)
        replaceCFGExpression(line);
      content += line + '\n';
    }
  }

  fs.close();

  return content;
}

/**
 * @brief
 *   Creates an empty shader
 */
Shader::Shader() : Logging::Log("Shader"), mId(0), mFilename("Unknown") {}

/**
 * @brief
 *   Creates a shader from a file, assuming that the filename tells
 *   which type of shader it is:
 *
 *   vs = vertex shader
 *   fs = fragment shader
 *
 * @param filename
 */
Shader::Shader(const std::string& filename)
    : Logging::Log("Shader"), mFilename(filename) {
  mId       = loadShader(filename);
  mFilename = filename;
}

Shader::~Shader() {
  if (mId == 0)
    return;

  glDeleteShader(mId);
  mId = 0;
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

GLuint Shader::loadShader(const std::string& filename) {
  GLuint shaderType = GL_VERTEX_SHADER;

  if (filename.find(".fs") != std::string::npos)
    shaderType = GL_FRAGMENT_SHADER;

  mId = glCreateShader(shaderType);

  std::string shaderCodeStr = loadTextfile(filename);
  const char* source        = shaderCodeStr.c_str();

  glShaderSource(mId, 1, &source, 0);
  glCompileShader(mId);

  if (!checkShader()) {
    throw std::runtime_error("Error in shader. See below.");
  }

  return mId;
}

bool Shader::checkShader() {
  GLint isCompiled = 0;
  GLint maxLength  = 0;

  // retrieve the shader information
  glGetShaderiv(mId, GL_COMPILE_STATUS, &isCompiled);
  glGetShaderiv(mId, GL_INFO_LOG_LENGTH, &maxLength);

  // throw error if it cant compile
  if (isCompiled == GL_FALSE) {
    std::vector<GLchar> infoLog(maxLength);
    std::string         s = mFilename + ": ";

    glGetShaderInfoLog(mId, maxLength, &maxLength, &infoLog[0]);
    glDeleteShader(mId);

    mLog->error("Errors in shader: {} - see below:", mFilename);
    for (unsigned int i = 0; i < infoLog.size(); i++) {
      if (infoLog[i] == '\n') {
        mLog->error(s);
        s = mFilename + ": ";
      } else
        s += infoLog[i];
    }

    return false;
  }

  // if there some information
  if (maxLength > 1) {
    std::vector<GLchar> infoLog(maxLength);
    std::string         s = "";

    glGetShaderInfoLog(mId, maxLength, &maxLength, &infoLog[0]);

    for (unsigned int i = 0; i < infoLog.size(); i++)
      s += infoLog[i];

    mLog->warn("Warning in shader: {} - see below:",
               mFilename.c_str(),
               s.c_str());
  }

  return true;
}
