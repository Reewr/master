#pragma once

#include <map>
#include <memory>
#include <string>
#include <type_traits>

#include "../Utils/Utils.hpp"
#include "Resource.hpp"

class ResourceManager {
public:
  ResourceManager();
  ~ResourceManager();

  template <typename T>
  std::shared_ptr<T> get(const std::string& name);

  void loadDescription(const std::string& filename);

  void loadRequired(ResourceScope scope);
  void unloadAll();
  void unloadUnnecessary(ResourceScope scope);

private:
  ResourceScope mCurrentScope;
  std::map<std::string, std::shared_ptr<Resource>> mResources;
};

// ----------------------------------------------------------
//
// Below follows the implementation of the template functions
//
// ----------------------------------------------------------

template <typename T>
std::shared_ptr<T> ResourceManager::get(const std::string& name) {
  if (mResources.count(name) == 0)
    throw std::runtime_error("Could not find filename for " + name);

  if (!mResources[name]->includesScope(mCurrentScope)) {
    throw std::runtime_error("'" + name + "' is not available for scope: '" +
                             std::to_string(static_cast<int>(mCurrentScope)) +
                             "'");
  }

  if (mResources[name]->loaded())
    return std::dynamic_pointer_cast<T>(mResources[name]);

  bool result = mResources[name]->load();

  if (!result)
    throw std::runtime_error("Failed to load resource: '" + name + "'");

  return std::dynamic_pointer_cast<T>(mResources[name]);
}
