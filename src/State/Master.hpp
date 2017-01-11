#ifndef STATE_MASTER_HPP
#define STATE_MASTER_HPP

#include "State.hpp"

class Input;
class GConsole;
struct Asset;

class Master : public State {
public:
  Master(Asset* asset, Input* input);

  ~Master();

  void update(float deltaTime);

  //! If keyboard input indicates that there is a state change, return int of
  //! state
  int keyboardCB(int key, int scan, int action, int mods);

  //!
  void mouseMovementCB(double x, double y);

  //! If mouse input indicates that there is a state change, returns int of
  //! state
  int mouseButtonCB(int button, int action, int mods);

  //!
  void mouseScrollCB(double offsetx, double offsety);

private:
  void draw3D();
  void drawGUI();

  Input*    mInput;
  Asset*    mAsset;
  GConsole* mConsole;
};
#endif
