#ifndef STATE_MAINMENU_HPP
#define STATE_MAINMENU_HPP

#include "State.hpp"

class Menu;
class Window;
class OptionsMenu;
class Texture;
class Framebuffer;
class Input;

//! Main Menu. Guess what it handles? :D
class MainMenu : public State {
public:
  //! Only constructor. Initalizes the menu
  MainMenu(Asset* asset, Input* input);

  //! Destructor
  ~MainMenu();

  //! Runs all the functions that needs updating (animations, draw etc)
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

  void charCB(unsigned int codepoint);

protected:
  int handleMenuActionInput();

  void draw3D();
  void drawGUI();

  Input*       mInput;
  Asset*       mAsset;
  Window*      mMenu;
  Window*      mBackground;
  OptionsMenu* mOptionsMenu;
};

#endif