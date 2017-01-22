#pragma once

#include <string>

/**
 * @brief
 *   This class represents a resource. Since the engine now uses a resource
 *   manager, all the resources should inherit from a single class to make
 *   storing them slightly easier.
 *
 *   The class holds information about where to find the resource on disk,
 *   which scopes it is available for and what type of resource it is.
 *
 *   All of these are used by the ResourceManager. In addition, it also
 *   implements two pure virtual functions that needs to be implemented
 *   when inheriting from this class.
 *
 *   **Note**: You should NOT make any expensive operations or allocations
 *   within the constructor of the resource inheritng from this class
 *
 *   All of these operations should be reserved for the `load` function.
 *   Similarily, all deallocation of the said resources should be reserved
 *   for the `unload` function.
 */
class Resource {
public:
  Resource();
  virtual ~Resource();

  //! Whenever the resource manager feels that this resource is
  //! required, it will load it using this function. If you
  //! require the filename, it can be accessed via the member
  //! variable or the getter function
  virtual bool load() = 0;

  //! Whenever the resource manager feels like this resource
  //! is useless, it will call unload. This function
  //! should unload any resources it requires.
  virtual void unload() = 0;

  //! Sets the type of the resource. The types
  //! can be find as enums in the ResourceManager
  void setType(unsigned int type);

  //! Sets the scope of the resource. As with type,
  //! this can also be found in the ResourceManager
  void setScope(unsigned int scope);

  //! Sets where the resource is located on disk
  void setFilename(const std::string& filename);

  //! Sets the name of the resource, which is
  //! also stored by the ResourceManager
  //! to be used when looking up a resource.
  void setName(const std::string& name);

  // Sets whether or not the resource has been loaded
  void setLoaded(bool loaded);

  //! Retrieve type
  unsigned int type();

  //! Retrieve scope
  unsigned int scope();

  //! Checks if the resource is within a scope. Since
  //! the scope is a FLAG, it will check whether
  //! the given scope is within its legal flaglist
  unsigned int includesScope(unsigned int scope);

  // Retrieve filename
  std::string filename();

  // Retrive name
  std::string name();

  // retrieve whether it is loaded or not
  bool loaded();

protected:

  unsigned int mScope;
  unsigned int mType;
  std::string  mName;
  std::string  mFilename;
  bool         mLoaded;
};
