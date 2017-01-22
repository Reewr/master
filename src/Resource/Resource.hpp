#pragma once

#include <string>

class Resource {
public:
  Resource();
  virtual ~Resource();

  virtual bool load() = 0;
  virtual void unload() = 0;

  void setType(unsigned int type);
  void setScope(unsigned int scope);

  void setFilename(const std::string& filename);
  void setName(const std::string& name);

  void setLoaded(bool loaded);

  unsigned int type();
  unsigned int scope();

  unsigned int includesScope(unsigned int scope);

  std::string filename();
  std::string name();

  bool loaded();

protected:

  unsigned int mScope;
  unsigned int mType;
  std::string  mName;
  std::string  mFilename;
  bool         mLoaded;
};
