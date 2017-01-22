#include "ResourceManager.hpp"
#include "../Utils/Utils.hpp"

#include "../GLSL/Program.hpp"
#include "../Graphical/Texture.hpp"
#include "../Graphical/Font.hpp"

#include <sol.hpp>

ResourceManager::ResourceManager() : mCurrentScope(0) { }

ResourceManager::~ResourceManager() {
  for(auto &rPair : mResources) {
    if (rPair.second->loaded())
      rPair.second->unload();
  }
}

void ResourceManager::loadRequired(int scope) {
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

void ResourceManager::unloadUnnecessary(int scope) {
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

  lua.create_named_table("ResourceType",
      "None", ResourceTypeNone,
      "Texture", ResourceTypeTexture,
      "Program", ResourceTypeProgram,
      "Font", ResourceTypeFont);

  lua.create_named_table("ResourceScope",
      "MainMenu", ResourceScopeMainMenu,
      "Game", ResourceScopeGame,
      "Master", ResourceScopeMaster,
      "All", ResourceScopeAll);

  lua
    .set_function("addResource",
                  [&](std::string name,
                      std::string path,
                      int         type,
                      int         scope) {
                    bool existsButDifferent =
                      mResources.count(name) &&
                      mResources[name]->filename() != path;
                    if (existsButDifferent) {
                      throw std::invalid_argument(
                        "Name already in resourcename with different filename");
                    }

                    switch (type) {
                      case ResourceTypeProgram:
                        mResources[name] = std::shared_ptr<Resource>(new Program());
                        break;
                      case ResourceTypeFont:
                        mResources[name] = std::shared_ptr<Resource>(new Font());
                        break;
                      case ResourceTypeTexture:
                        mResources[name] = std::shared_ptr<Resource>(new Texture());
                        break;
                      default:
                        throw std::runtime_error("ResourceType implementation does not exist");
                    }

                    mResources[name]->setFilename(path);
                    mResources[name]->setScope(scope);
                    mResources[name]->setType(type);
                    log("ResourceManager :: Added '", name, "'");
                  });

  log("ResourceManager :: Running script '", filename, "'");
  lua.script_file(filename);
}
