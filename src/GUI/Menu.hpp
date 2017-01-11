#ifndef GUI_MENU_HPP
#define GUI_MENU_HPP

#include <vector>

#include "../Graphical/Text.hpp"
#include "GUI.hpp"

namespace tinyxml2 {
class XMLElement;
}

class Menu : public GUI {
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

  //! Constructor that creates a menu item with name at pos with character size:
  //! zie
  Menu(const std::string   name,
       const vec2&         pos,
       const MenuSettings& m = MenuSettings());

  //! Same as constructor above, but several names. Increments by 75 in Y
  //! position for each element
  Menu(const std::vector<std::string>& names,
       const vec2&                     startPos,
       const MenuSettings&             m = MenuSettings());

  static Menu* fromXML(tinyxml2::XMLElement* element);

  //! Deconstructor
  ~Menu();

  //! :)
  void draw(float deltaTime) const;

  //! Returns index if inside any of the menuItems, -1 otherwise.
  int isInsideMenuElement(const vec2& pos) const;

  //! Adds 1 menu item at pos
  void addMenuItem(const std::string   name,
                   const vec2&         pos,
                   const MenuSettings& m = MenuSettings());

  //! Uses addMenuItem to add several. Increments by 75 in Y position
  void addMenuItems(const std::vector<std::string>& names,
                    const vec2&                     startPos,
                    const MenuSettings&             m = MenuSettings());

  //! Sets the active menuItem - turns it yellow
  void setActiveMenu(const int i);

  //! Does the same as above, but with keyboard
  void setActiveMenuKeyboard(const int key);

  //! Returns the index of the active menu item
  int getActiveMenu() const;

  //! Sets an offset on all menu items
  void setOffset(const vec2& offset);

private:
  float              mAnimationTime;
  int                mActiveMenu;
  vec2               mAnimationDistance;
  std::vector<Text*> mMenuItems;
};

#endif