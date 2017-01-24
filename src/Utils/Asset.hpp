#pragma once

namespace Input {
  class Input;
}

namespace Lua {
  class Lua;
}

class CFG;
class ResourceManager;

//! Asset is a class that is being sent around that stores a lot of useful
//! settings.
class Asset {
public:
  Asset(CFG* c);

  CFG*             cfg();
  Input::Input*    input();
  Lua::Lua*        lua();
  ResourceManager* rManager();

  void setCFG(CFG* c);
  void setInput(Input::Input* i);
  void setLua(Lua::Lua* lua);
  void setResourceManager(ResourceManager* r);

private:
  CFG*             mCFG             = nullptr;
  Input::Input*    mInput           = nullptr;
  Lua::Lua*        mLua             = nullptr;
  ResourceManager* mResourceManager = nullptr;
};