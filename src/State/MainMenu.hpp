#pragma once

#include "State.hpp"

class Menu;
class Window;
class OptionsMenu;
class Texture;
class Framebuffer;

namespace Input {
class Input;
class Event;
}

//! Main Menu. Guess what it handles? :D
class MainMenu : public State {
public:
  //! Only constructor. Initalizes the menu
  MainMenu(Asset* asset);

  //! Destructor
  ~MainMenu();

  //! Runs all the functions that needs updating (animations, draw etc)
  void update(float deltaTime);
  void draw(float deltaTime);

  void input(const Input::Event& event);

protected:
  int handleMenuActionInput();

  void draw3D();
  void drawGUI();

  Asset* mAsset;
};