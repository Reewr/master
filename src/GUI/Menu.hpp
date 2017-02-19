#pragma once

#include <vector>

#include "../Log.hpp"
#include "GUI.hpp"
#include "Text.hpp"

namespace tinyxml2 {
  class XMLElement;
}

class Menu : public GUI, public Logging::Log {
public:
  struct MenuSettings {
    MenuSettings(float size   = 20,
                 float offset = 50,
                 int   ori    = Menu::VERTICAL,
                 int   color  = Text::WHITE);
    float size;
    float offset;
    int   ori;
    int   color;
  };

  enum { HORIZONTAL, VERTICAL };

  //! Default Constructor - does nothing
  Menu();

  //! Constructor that creates a menu item with name at pos with character size
  Menu(const std::string&  name,
       const mmm::vec2&    pos,
       const MenuSettings& m = MenuSettings());

  //! Same as constructor above, but several names. Increments by 75 in Y
  //! position for each element
  Menu(const std::vector<std::string>& names,
       const mmm::vec2&                startPos,
       const MenuSettings&             m = MenuSettings());

  static Menu* fromXML(tinyxml2::XMLElement* element);

  //! Deconstructor
  ~Menu();

  //! :)
  void draw();

  //! Can be called to do the default actions by using
  //! setInputHandler. Is also called by default if setInputHandler
  //! is never called.
  void defaultInputHandler(const Input::Event& event);

  //! Returns index if inside any of the menuItems, -1 otherwise.
  int isInsideMenuElement(const mmm::vec2& pos) const;

  //! Adds 1 menu item at pos
  void addMenuItem(const std::string&  name,
                   const mmm::vec2&    pos,
                   const MenuSettings& m = MenuSettings());

  //! Uses addMenuItem to add several. Increments by 75 in Y position
  void addMenuItems(const std::vector<std::string>& names,
                    const mmm::vec2&                startPos,
                    const MenuSettings&             m = MenuSettings());

  //! Removes the menu items from the menu
  void clearMenuItems();

  //! Sets the active menuItem - turns it yellow
  bool setActiveMenu(const int i);

  //! Does the same as above, but with keyboard
  bool setActiveMenuKeyboard(const int key);

  //! Returns the index of the active menu item
  int getActiveMenu() const;

  //! Returns the Text element that is active, otherwise nullptr
  const Text* getActiveMenuItem() const;

  //! Sets an offset on all menu items
  void setOffset(const mmm::vec2& offset);

private:
  int                mActiveMenu;
  mmm::vec2          mAnimationDistance;
  std::vector<Text*> mMenuItems;
};
