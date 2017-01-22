#include "Resource.hpp"

Resource::Resource() : mScope(0), mType(-1), mName(""), mFilename("") {}

Resource::~Resource() {}

void Resource::setType(unsigned int type) {
  mType = type;
}

void Resource::setScope(unsigned int scope) {
  mScope = scope;
}

void Resource::setFilename(const std::string& filename) {
  mFilename = filename;
}

void Resource::setName(const std::string& name) {
  mName = name;
}

void Resource::setLoaded(bool loaded) {
  mLoaded = loaded;
}

unsigned int Resource::includesScope(unsigned int scope) {
  return mScope & scope;
}

unsigned int Resource::type() {
  return mType;
}

unsigned int Resource::scope() {
  return mScope;
}

std::string Resource::filename() {
  return mFilename;
}

std::string Resource::name() {
  return mName;
}

bool Resource::loaded() {
  return mLoaded;
}
