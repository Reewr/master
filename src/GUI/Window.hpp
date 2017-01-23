#pragma once

#include <list>
#include <map>
#include <string>
#include <vector>

#include "GUI.hpp"
#include "Menu.hpp"

class Slider;
class Text;
class Dropdown;
class Checkbox;
class Inputbox;

class GLRectangle;

namespace tinyxml2 {
  class XMLElement;
}

//! Window is a GUI class that creates a block to act as a window on the screen
class Window : public GUI {
public:
  Window();

  Window(std::string texFilename, Rectangle r);

  static Window* fromXML(tinyxml2::XMLElement* element);

  ~Window();

  void setOffset(const vec2& offset);

  // :)
  void draw();

  //! Can be called to do the default actions by using
  //! setInputHandler. Is also called by default if setInputHandler
  //! is never called.
  void defaultInputHandler(const Input::Event& event);

  //! Adds a title to the middle-top of the window
  void addTitle(std::string s);

  void add(std::string name, GUI* widget);

  //! Add a GUI object to the window.
  void addWindow(std::string name, Rectangle r, std::string tex = "NONE");
  void addMenu(std::string                     name,
               const std::vector<std::string>& names,
               const vec2&                     startPos,
               const Menu::MenuSettings&       m = Menu::MenuSettings());

  void addMenuItem(std::string               name,
                   std::string               text,
                   const vec2&               pos,
                   const Menu::MenuSettings& m = Menu::MenuSettings());

  void addSlider(std::string name, vec2 pos, float scale = 1);
  void addDropdown(std::string name, std::vector<std::string> opt, vec2 pos);
  void addCheckbox(std::string name, vec2 pos);
  void addInputbox(std::string name, Rectangle r, std::string text = "NOT SET");

  void setActiveMenuItem(std::string name, vec2 pos);

  //! Returns a GUI object based on name. If it doesn't exist, returns NULL
  Menu* menu(std::string name);
  Slider* slider(std::string name);
  Dropdown* dropdown(std::string name);
  Checkbox* checkbox(std::string name);
  Inputbox* inputbox(std::string name);
  Window* window(std::string name);

  //! Returns a map of different things
  std::map<std::string, Menu*>     menues();
  std::map<std::string, Slider*>   sliders();
  std::map<std::string, Dropdown*> dropdowns();
  std::map<std::string, Checkbox*> checkboxes();
  std::map<std::string, Inputbox*> inputboxes();
  std::map<std::string, Window*>   windows();

  virtual int handleKeyInput(int key, int action);
  virtual int handleMouseButton(int button, int action);
  virtual int handleAction();

  bool hasChanged() const;
  void hasChanged(bool c);

protected:
  void sortDropdowns();

  GLRectangle* mBackground;
  Text*        mTitle;

  std::list<Dropdown*> mDrawDropdowns;

  std::map<std::string, Menu*>     mMenues;
  std::map<std::string, Slider*>   mSliders;
  std::map<std::string, Window*>   mWindows;
  std::map<std::string, Dropdown*> mDropdowns;
  std::map<std::string, Checkbox*> mCheckboxes;
  std::map<std::string, Inputbox*> mInputboxes;
};
