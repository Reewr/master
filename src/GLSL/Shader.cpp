#include "Shader.hpp"
#include <fstream>
#include <vector>

#include "../Utils/CFG.hpp"
#include "../Utils/Utils.hpp"
#include "../Utils/str.hpp"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined(__CYGWIN__)
#include <windows.h>
#else
#include <limits.h>
#include <stdlib.h>
#endif

CFG* Shader::mCFG = nullptr;

// this variable contains the seperators that `findFirstSeperator`
// will look for.
const std::string SHADER_CFG_SEPARATORS = ";+*/-, ";

/**
 * @brief
 *   Queries the system for the absolute path of the file so that
 *   we can correctly join included files together. This also
 *   helps in understanding whether a file has been included or not,
 *   as all paths will be absolute.
 *
 * @param filename
 *
 * @return
 */
std::string getAbsoluteDir(const std::string& filename) {
// If on windows, use windows absolute path function, otherwise assume
// we are on a Linux/OSD variant and use realpath
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined(__CYGWIN__)
  TCHAR fullPath[MAX_PATH];
  GetFullPathName(_T(filename), MAX_PATH, fullPath, NULL);
#else
  char fullPath[PATH_MAX];
  realpath(filename.c_str(), fullPath);
#endif
  std::string absPath   = std::string(fullPath);
  size_t      lastSlash = absPath.find_last_of("/");

  if (lastSlash != std::string::npos) {
    return absPath.substr(0, lastSlash);
  }

  return absPath;
}

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
 *   Replaces a line that contains a layout binding to code that is supported
 *   by lower GLSL versions.
 *
 *   This is primarily to support the Ultrabooks that we use, as these does not
 *   yet support OpenGL 4.5+
 *
 *   The lines that are replaced are lines such as:
 *
 *   `layout(binding=0) uniform sampler2D shadowmap;`
 *
 *   which are turned into:
 *
 *   `uniform sampler2D shadowmap;`
 *
 *   The location and name of the uniform is stored so that the
 *   program can set these correctly when initalizing the program.
 *
 * @param line
 *
 * @return
 */
Shader::LayoutBinding replaceLayoutBinding(std::string& line) {
  size_t pos       = line.find("layout(binding=");
  size_t semicolon = line.find(";");

  if (semicolon == std::string::npos)
    throw std::runtime_error("Layout binding without ending semicolon");

  size_t startVarName = line.rfind(" ", semicolon);
  size_t endLayoutNum = line.find(")");

  if (endLayoutNum <= pos)
    throw std::runtime_error("Layout binding with bracket before 'layout'");

  Shader::LayoutBinding layout;

  std::string num  = line.substr(pos + 15, endLayoutNum);
  std::string name = line.substr(startVarName, semicolon - startVarName);
  std::string rest = line.substr(endLayoutNum + 1);

  try {
    layout.location = std::stoi(num);
  } catch (const std::invalid_argument& a) {
    throw std::runtime_error(a.what());
  }

  layout.name = name;

  str::trim(layout.name);
  str::trim(rest);

  line = rest;

  return layout;
}

/**
 * @brief
 *   Assumes that the line has an `include` directive and its job
 *   is to retrieve the filename in between two tags.
 *
 *   The tags that work are "" and <>, any other type of tags
 *   will cause this function to throw an error.
 *
 *   Returns the string that is enclosed in between these two tags.
 *
 * @param line
 *
 * @return
 */
std::string getIncludeFilename(const std::string& line) {
  bool hasInclude = line.find("#include") != std::string::npos;

  if (!hasInclude)
    throw std::runtime_error("The line has no include directive");

  size_t firstSign = line.find("<");
  size_t lastSign  = line.find_last_not_of(">");

  // Handle if the include uses <> tags
  if (firstSign != std::string::npos) {
    if (lastSign == std::string::npos)
      throw std::runtime_error("Line is missing end delimiter '>': " + line);

    return line.substr(firstSign + 1, lastSign - firstSign - 1);
  }

  size_t firstQuote = line.find("\"");
  size_t lastQuote  = line.find_last_of("\"");

  // Handle if the include uses "" tags
  if (firstQuote != std::string::npos) {
    if (lastQuote == std::string::npos)
      throw std::runtime_error("Line is missing end delimiter '\"': " + line);

    return line.substr(firstQuote + 1, lastQuote - firstQuote - 1);
  }

  // Throw since we didnt find any valid tags
  throw std::runtime_error("Include directive uses incorrect symbols");
}

/**
 * @brief
 *   Loads the shader from file, handling any special syntax as it detects it.
 *   This function supports two special syntaxes:
 *
 *   1. "#include"
 *   2. "_CFG_"
 *
 *   The first, `#include`, will let you include a file from file in its
 *   entirety. Please keep in mind that this is simple replace `include`
 *   with whole file it is referencing.
 *
 *   Keep in mind that include does not do anything fancy other than include the
 *   entire file into the same file. This may cause errors if function names are
 * defined
 *   multiple times. The parser will try its best to not include a file more
 * than once,
 *   even if there are multiple include statements.
 *
 *   Also keep in mind that when including files, you have to include it in the
 * order it
 *   is suppose to be used.
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
 * @param detail
 *
 * @return
 */
Shader::Details loadTextfile(const std::string& filename,
                             Shader::Details    detail = Shader::Details()) {

  std::ifstream fs(filename);
  std::string   line;
  std::string   absDir = getAbsoluteDir(filename);

  if (!fs.is_open()) {
    throw std::runtime_error("Unable to open file: " + filename);
  }

  while (fs.good()) {
    std::getline(fs, line);
    bool hasInclude       = line.find("#include") == 0;
    bool hasCFGExpression = line.find("_CFG_") != std::string::npos;
    bool hasLayoutBinding = line.find("layout(binding=") != std::string::npos;

    if (hasInclude) {
      // check if the file has already been included
      std::string includeFile = str::joinPath(absDir, getIncludeFilename(line));
      bool        hasBeenIncluded = false;

      for (auto& s : detail.includedFiles) {
        if (s == includeFile) {
          hasBeenIncluded = true;
          break;
        }
      }

      if (!hasBeenIncluded) {
        detail.includedFiles.push_back(includeFile);
        detail = loadTextfile(includeFile, detail);
      }

    } else {
      if (hasCFGExpression)
        replaceCFGExpression(line);
      if (hasLayoutBinding)
        detail.layoutBindings.push_back(replaceLayoutBinding(line));
      detail.source += line + '\n';
    }
  }

  fs.close();

  return detail;
}

/**
 * @brief
 *   Takes a string that represents the filename of the shader and
 *   checks which file extension the file has. If the file extension
 *   is recognized as a legal extension, the Shader type is returned.
 *
 *   If the file extension is not recognized, Shader::Type::None
 *   is returned.
 *
 * @param filename
 *
 * @return
 */
Shader::Type Shader::typeFromFilename(const std::string& filename) {
  if (filename.find(".fs") != std::string::npos) {
    return Shader::Type::Fragment;
  }

  if (filename.find(".vs") != std::string::npos) {
    return Shader::Type::Vertex;
  }

  if (filename.find(".gs") != std::string::npos) {
    return Shader::Type::Geometry;
  }

  if (filename.find(".ts") != std::string::npos) {
    return Shader::Type::Tessellation;
  }

  if (filename.find(".es") != std::string::npos) {
    return Shader::Type::Evaluation;
  }

  if (filename.find(".cs") != std::string::npos) {
    return Shader::Type::Compute;
  }

  return Shader::Type::None;
}

/**
 * @brief
 *   Stringifies the shader type
 *
 * @param t
 *
 * @return
 */
std::string Shader::typeToStr(Shader::Type t) {
  switch (t) {
    case Shader::Type::Fragment:
      return "Fragment";
    case Shader::Type::Vertex:
      return "Vertex";
    case Shader::Type::Geometry:
      return "Geometry";
    case Shader::Type::Tessellation:
      return "Tessellation";
    case Shader::Type::Evaluation:
      return "Evaluation";
    case Shader::Type::Compute:
      return "Compute";
    case Shader::Type::None:
      return "None";
  }
}

/**
 * @brief
 *   Returns the OpenGL type that the Shader type represents
 *
 * @param t
 *
 * @return
 */
GLuint Shader::typeToGLType(Type t) {
  switch (t) {
    case Shader::Type::Fragment:
      return GL_FRAGMENT_SHADER;
    case Shader::Type::Vertex:
      return GL_VERTEX_SHADER;
    case Shader::Type::Geometry:
      return GL_GEOMETRY_SHADER;
    case Shader::Type::Tessellation:
      return 0; // GL_TESS_CONTROL_SHADER;
    case Shader::Type::Evaluation:
      return 0; // GL_TESS_EVALUATION_SHADER;
    case Shader::Type::Compute:
      return 0; // GL_COMPUTE_SHADER;
    case Shader::Type::None:
      return 0;
  }
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
 * @param type
 */
Shader::Shader(const std::string& filename, Shader::Type type)
    : Logging::Log("Shader"), mFilename(filename) {
  loadShader(filename, type);
}

Shader::~Shader() {
  if (mId == 0)
    return;

  glDeleteShader(mId);
  mId = 0;
}

const Shader::Details& Shader::details() const {
  return mDetail;
}

std::string Shader::filename() const {
  return mFilename;
}

Shader::Type Shader::type() const {
  return mType;
}

GLuint Shader::id() const {
  return mId;
}

GLuint Shader::loadShader(const std::string& filename, Shader::Type type) {
  if (type == Shader::Type::None) {
    type = Shader::typeFromFilename(filename);
  }

  if (type == Shader::Type::None) {
    throw std::runtime_error("No type given to shader: " + filename);
  }

  GLuint shaderType = Shader::typeToGLType(type);
  mType             = type;
  mId               = glCreateShader(shaderType);
  mDetail           = loadTextfile(filename);

  const char* source = mDetail.source.c_str();

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

    glGetShaderInfoLog(mId, maxLength, &maxLength, &infoLog[0]);
    glDeleteShader(mId);

    std::string s(infoLog.begin(), infoLog.end());
    mLog->error("Errors in shader: {} - see below: \n{}", mFilename, s);
    mLog->info("For dev purposes, source code:");

    auto source = str::split(mDetail.source, '\n');

    int i = 1;
    for (auto& a : source) {
      std::string lineNum = std::to_string(i);

      if (lineNum.length() < 4) {
        lineNum = lineNum + std::string(" ", 4 - lineNum.length());
      }

      std::cout << "#" << lineNum << "  " << a << std::endl;
      i++;
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
