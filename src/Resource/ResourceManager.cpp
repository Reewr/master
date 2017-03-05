#include "ResourceManager.hpp"

#include "../GLSL/Program.hpp"
#include "../Lua/Lua.hpp"
#include "../Utils/Utils.hpp"
#include "Font.hpp"
#include "Mesh.hpp"
#include "Texture.hpp"
#include "PhysicsMesh.hpp"

ResourceManager::ResourceManager()
    : Logging::Log("ResManager"), mCurrentScope(ResourceScope::None) {}

ResourceManager::~ResourceManager() {
  unloadAll();
}

void ResourceManager::loadRequired(ResourceScope scope) {
  mLog->debug("Loading required for scope: '{}'", static_cast<int>(scope));
  mCurrentScope = scope;
  for (auto& rPair : mResources) {
    if (rPair.second->includesScope(scope) && !rPair.second->loaded()) {
      mLog->debug("Loading: '{}'", rPair.second->filename());
      rPair.second->load(this);
      rPair.second->setLoaded(true);
    } else if (rPair.second->includesScope(scope) && rPair.second->loaded()) {
      mLog->debug("Already loaded: '{}'", rPair.second->filename());
    } else {
      mLog->debug("Not loading: '{}'", rPair.second->filename());
    }
  }
}

void ResourceManager::unloadAll() {
  for (auto& rPair : mResources) {
    if (rPair.second->loaded()) {
      rPair.second->unload();
      rPair.second->setLoaded(false);
    }
  }
}

void ResourceManager::unloadUnnecessary(ResourceScope scope) {
  mCurrentScope = scope;
  for (auto& rPair : mResources) {
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
      "Empty"      , ResourceType::Empty,
      "Texture"    , ResourceType::Texture,
      "Program"    , ResourceType::Program,
      "Font"       , ResourceType::Font,
      "Mesh"       , ResourceType::Mesh,
      "PhysicsMesh", ResourceType::PhysicsMesh);

  lua.create_named_table("ResourceScope",
      "MainMenu", ResourceScope::MainMenu,
      "Game"    , ResourceScope::Game,
      "Master"  , ResourceScope::Master,
      "All"     , ResourceScope::All);
  // clang-format on

  auto addResource = [&](std::string   name,
                         std::string   path,
                         ResourceType  type,
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
      case ResourceType::Mesh:
        mResources[name] = std::shared_ptr<Resource>(new class Mesh());
        break;
      case ResourceType::PhysicsMesh:
        mResources[name] = std::shared_ptr<Resource>(new class PhysicsMesh());
        break;
      default:
        throw std::runtime_error("ResourceType implementation does not exist");
    }

    mResources[name]->setFilename(path);
    mResources[name]->setScope(scope);
    mResources[name]->setType(type);
    mResources[name]->setName(name);
    mLog->debug("Added '{}'", name);
  };

  lua.set_function("addResource", addResource);

  mLog->debug("Running script '{}'", filename);
  lua.script_file(filename);
}
