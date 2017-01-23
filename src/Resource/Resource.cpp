#include "Resource.hpp"

Resource::Resource()
    : mScope(ResourceScope::None)
    , mType(ResourceType::Empty)
    , mName("")
    , mFilename("")
    , mLoaded(false) {}

Resource::~Resource() {}

/**
 * @brief
 *   Sets the type to an enum that can be found in
 *   the ResourceManager class
 *
 * @param type
 */
void Resource::setType(ResourceType type) {
  mType = type;
}

/**
 * @brief
 *   Sets the scope of the resource. The scope is a
 *   list of flags, which signals which scope it
 *   should be available in.
 *
 * @param scope
 */
void Resource::setScope(ResourceScope scope) {
  mScope = scope;
}

/**
 * @brief
 *   Sets the filename, or the path to where it
 *   can be found on disk in relation to the executable.
 *
 * @param filename
 */
void Resource::setFilename(const std::string& filename) {
  mFilename = filename;
}

/**
 * @brief
 *   Sets a unique name for the resource. This is
 *   used for looking up a resource.
 *
 * @param name
 */
void Resource::setName(const std::string& name) {
  mName = name;
}

/**
 * @brief
 *   Returns whether or not a resource has been loaded
 *
 * @param loaded
 */
void Resource::setLoaded(bool loaded) {
  mLoaded = loaded;
}

/**
 * @brief
 *   Checks if a given scope is included in the resource flags
 *
 * @param scope
 *
 * @return
 */
bool Resource::includesScope(ResourceScope scope) {
  return int(mScope & scope) != 0;
}

/**
 * @brief
 *   Returns the type of the resource.
 *
 * @return
 */
ResourceType Resource::type() {
  return mType;
}

/**
 * @brief
 *   Returns the scope of the resource
 *
 * @return
 */
ResourceScope Resource::scope() {
  return mScope;
}

/**
 * @brief
 *   Returns the filename / path to the resource
 *   on disk
 *
 * @return
 */
std::string Resource::filename() {
  return mFilename;
}

/**
 * @brief
 *   Returns what it suppose to be a unique name
 *
 * @return
 */
std::string Resource::name() {
  return mName;
}

/**
 * @brief
 *   Returns a boolean representing whether or not
 *   the resource has been loaded
 *
 * @return
 */
bool Resource::loaded() {
  return mLoaded;
}
