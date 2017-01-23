#include "ResourceManager.hpp"
#include "../Utils/Utils.hpp"

#include "../GLSL/Program.hpp"
#include "Texture.hpp"
#include "../Graphical/Font.hpp"

#include <sol.hpp>

ResourceManager::ResourceManager() : mCurrentScope(ResourceScope::None) { }

ResourceManager::~ResourceManager() {
  for(auto &rPair : mResources) {
    if (rPair.second->loaded())
      rPair.second->unload();
  }
}

void ResourceManager::loadRequired(ResourceScope scope) {
  mCurrentScope = scope;
  for(auto &rPair : mResources) {
    if (rPair.second->includesScope(scope) && !rPair.second->loaded()) {
      rPair.second->load();
      rPair.second->setLoaded(true);
    }
  }
}

void ResourceManager::unloadAll() {
  for(auto &rPair : mResources) {
    rPair.second->unload();
    rPair.second->setLoaded(false);
  }
}

void ResourceManager::unloadUnnecessary(ResourceScope scope) {
  mCurrentScope = scope;
  for(auto &rPair : mResources) {
    if (!rPair.second->includesScope(scope) && rPair.second->loaded()) {
      rPair.second->unload();
      rPair.second->setLoaded(false);
    }
  }
}

void ResourceManager::loadDescription(const std::string& filename) {
  sol::state lua;

  lua.open_libraries(sol::lib::base, sol::lib::bit32);

  // clang-format off
  lua.create_named_table("ResourceType",
      "Empty"  , ResourceType::Empty,
      "Texture", ResourceType::Texture,
      "Program", ResourceType::Program,
      "Font"   , ResourceType::Font);

  lua.create_named_table("ResourceScope",
      "MainMenu", ResourceScope::MainMenu,
      "Game"    , ResourceScope::Game,
      "Master"  , ResourceScope::Master,
      "All"     , ResourceScope::All);
  // clang-format on

  auto addResource = [&](std::string name,
                         std::string path,
                         ResourceType type,
                         ResourceScope scope) {
    bool existsButDifferent =
      mResources.count(name) && mResources[name]->filename() != path;
    if (existsButDifferent) {
      throw std::invalid_argument(
        "Name already in resourcename with different filename");
    }

    switch (type) {
      case ResourceType::Program:
        mResources[name] = std::shared_ptr<Resource>(new class Program());
        break;
      case ResourceType::Font:
        mResources[name] = std::shared_ptr<Resource>(new class Font());
        break;
      case ResourceType::Texture:
        mResources[name] = std::shared_ptr<Resource>(new class Texture());
        break;
      default:
        throw std::runtime_error("ResourceType implementation does not exist");
    }

    mResources[name]->setFilename(path);
    mResources[name]->setScope(scope);
    mResources[name]->setType(type);
    log("ResourceManager :: Added '", name, "'");
  };

  lua.set_function("addResource", addResource);

  log("ResourceManager :: Running script '", filename, "'");
  lua.script_file(filename);
}
