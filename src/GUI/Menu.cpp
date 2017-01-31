#include "Menu.hpp"

#include "../OpenGLHeaders.hpp"
#include <tinyxml2.h>

#include "../Input/Event.hpp"
#include "../Utils/Asset.hpp"
#include "../Utils/Utils.hpp"
#include "Text.hpp"

#include <limits>

using mmm::vec2;

Menu::MenuSettings::MenuSettings(float size, float offset, int ori, int color) {
  this->size   = size;
  this->offset = offset;
  this->ori    = ori;
  this->color  = color;
}

/**
 * @brief
 *   Initializes an empty menu
 */
Menu::Menu() {
  mActiveMenu = -1;
  isVisible(true);
}

/**
 * @brief
 *   Initializes a menu with one item.
 *   Only color and size is used from menu settings
 *   in this case.
 *
 * @param text
 *   Text to display
 *
 * @param position
 *   The position of the element
 *
 * @param m
 *   The settings for the menu item
 */
Menu::Menu(const std::string&  text,
           const vec2&         position,
           const MenuSettings& m) {
  mActiveMenu = -1;
  isVisible(true);
  addMenuItem(text, position, m);
}

/**
 * @brief
 *   Initializes a menu with several items. Uses all
 *   the settings of the MenuSettings
 *
 * @param text
 *   Text to display
 *
 * @param startPosition
 *   Start position of the elements,
 *   going either left or down depending on
 *   the settings
 *
 * @param m
 *   The settings for the menu items
 */
Menu::Menu(const std::vector<std::string>& names,
           const vec2&                     startPosition,
           const MenuSettings&             m) {
  mActiveMenu = -1;
  isVisible(true);
  addMenuItems(names, startPosition, m);
}

/**
 * @brief
 *   Creates the menu from XML
 *
 *   Syntax:
 *
 *   <guimenu name="Graphics" x="55" y="35" size="15" ori="1" offset="0"
 * color="1">
 *     <item name="Resolution x="55" y="35"/>
 *     <item name="Vsync x="225" y="35"/>
 *     <item name="Display Mode x="575" y="35"/>
 *   </guimenu>
 *
 * @param element
 *
 * @return
 */
Menu* Menu::fromXML(tinyxml2::XMLElement* element) {
  if (element == nullptr) {
    throw std::runtime_error("XMLElement is null");
  }

  // retrieve the position from the XML element
  auto getPosition = [](tinyxml2::XMLElement* innerElement) -> vec2 {
    vec2 position;

    if (innerElement->QueryFloatAttribute("x", &position.x) != 0) {
      throw std::runtime_error("XMLElement has no float attribute 'x'");
    }

    if (innerElement->QueryFloatAttribute("y", &position.y) != 0) {
      throw std::runtime_error("XMLElement has no float attribute 'y'");
    }

    return position;
  };

  // retrieve the menu settins, setting default if they dont
  // exists
  auto getMenuSettings =
    [](tinyxml2::XMLElement* innerElement) -> MenuSettings {
    MenuSettings ms;

    if (innerElement->QueryFloatAttribute("size", &ms.size) != 0) {
      ms.size = 20;
    }

    if (innerElement->QueryFloatAttribute("offset", &ms.offset) != 0) {
      ms.offset = 50;
    }

    if (innerElement->QueryIntAttribute("ori", &ms.ori) != 0) {
      ms.ori = Menu::VERTICAL;
    }

    if (innerElement->QueryIntAttribute("color", &ms.color) != 0) {
      ms.color = Text::WHITE;
    }

    return ms;
  };

  // Menu settings
  MenuSettings mainMs       = getMenuSettings(element);
  vec2         mainPosition = getPosition(element);
  const char*  name         = element->Attribute("name");

  if (name == nullptr) {
    throw std::runtime_error("XMLElement has no attribute 'name'");
  }

  Menu* menu = new Menu(std::string(name), mainPosition, mainMs);
  tinyxml2::XMLElement* childElement = element->FirstChildElement();

  // Go through each child item, if they exist,
  // and do exactly the same as we did above
  for (; childElement != nullptr;
       childElement = childElement->NextSiblingElement()) {
    MenuSettings ms       = getMenuSettings(childElement);
    const char*  chName   = childElement->Attribute("name");
    vec2         position = getPosition(childElement);

    if (chName == nullptr) {
      throw std::runtime_error("XMLElement has no attribute 'name'");
    }

    menu->addMenuItem(std::string(chName), position, ms);
  }

  return menu;
}

Menu::~Menu() {
  clearMenuItems();
}

/**
 * @brief
 *
 *  The default handler for the Dropbox is called whenever input()
 *  is called. This can be overriden by setting a handler using
 *  setInputHandler()
 *
 *  The input handler set through this function can also
 *  call the default input handler, if the context is avaible.
 *
 * @param event
 *
 * @return
 */
void Menu::defaultInputHandler(const Input::Event& event) {
  if (!isVisible())
    return;

  if (event == Input::Event::Type::MouseMovement) {
    if (setActiveMenu(isInsideMenuElement(event.position())))
      event.stopPropgation();

    return;
  }

  if (event == Input::Event::Type::KeyPress) {
    if (setActiveMenuKeyboard(event.key()))
      event.stopPropgation();

    return;
  }
}

/**
 * @brief
 *   Adds a new menu item to the list, using the position
 *   given. Only uses the size and color of the menu
 *   settings.
 *
 * @param text
 *   Text to show
 *
 * @param position
 *   The screen position to use
 *
 * @param m
 *   The menu settings to use
 */
void Menu::addMenuItem(const std::string&  text,
                       const vec2&         position,
                       const MenuSettings& m) {
  if (mMenuItems.size() == 0)
    mBoundingBox.topleft = position;

  Text* item = new Text("Font::Dejavu", text, position, m.size);
  item->setColor(m.color);
  mMenuItems.push_back(item);
}

/**
 * @brief
 *   Adds several new menu items that will start
 *   at the start position, and then either be added
 *   right or downwards depending on the menu settings.
 *
 *   ALl menu settings are used in this case
 *
 * @param texts
 *   elements to add
 *
 * @param position
 *   The screen position to use
 *
 * @param m
 *   The menu settings to use
 */
void Menu::addMenuItems(const std::vector<std::string>& texts,
                        const vec2&                     startPosition,
                        const MenuSettings&             m) {
  for (unsigned int i = 0; i < texts.size(); i++) {
    float newX = startPosition.x;
    float newY = startPosition.y;

    if (mMenuItems.size() >= 1) {
      Text* lastElement = mMenuItems.back();
      vec2  bottomRight = lastElement->box().bottomright();

      if (i > 0 && m.ori == VERTICAL)
        newY = bottomRight.y + m.offset / 2;
      else if (i > 0 && m.ori == HORIZONTAL)
        newX = bottomRight.x + m.offset;
    }

    addMenuItem(texts[i], { newX, newY }, m);
  }

  vec2 bottomright = { 0, 0 };
  vec2 topleft     = { std::numeric_limits<float>::max(),
                   std::numeric_limits<float>::max() };

  // Update the size of the menu element.
  for (auto a : mMenuItems) {
    const Rectangle& r      = a->box();
    vec2             textBR = r.bottomright();

    if (r.topleft.x < topleft.x)
      topleft.x = r.topleft.x;

    if (r.topleft.y < topleft.y)
      topleft.y = r.topleft.y;

    bool isLargerX = textBR.x > bottomright.x;
    bool isLargerY = textBR.y > bottomright.y;

    if (isLargerX && isLargerY)
      bottomright = textBR;
    else if (isLargerX)
      bottomright.x = textBR.x;
    else if (isLargerY)
      bottomright.y = textBR.y;
  }

  mBoundingBox = { topleft.x,
                   topleft.y,
                   bottomright.x - topleft.x,
                   bottomright.y - topleft.y };
}

void Menu::clearMenuItems() {
  for (auto a : mMenuItems)
    delete a;

  mMenuItems.clear();
}

/**
 * @brief
 *   Checks whether a position is inside of the
 *   menu elements. Returns an index of the element
 *   it is inside, -1 if no match or if the menu isnt
 *   visible
 *
 * @param position
 *   screen position
 *
 * @return index
 */
int Menu::isInsideMenuElement(const vec2& position) const {
  if (!isVisible())
    return -1;

  for (unsigned int i = 0; i < mMenuItems.size(); i++) {
    if (mMenuItems[i]->isInside(position))
      return i;
  }

  return -1;
}

/**
 * @brief
 *   Sets which menu item is active, where an index
 *   of -1 resets it.
 *
 * @param index
 */
bool Menu::setActiveMenu(const int index) {
  if (!isVisible() && index != -1)
    return false;

  if (index >= (int) mMenuItems.size()) {
    return setActiveMenu(-1);
  }

  if (mActiveMenu != -1 && mActiveMenu != index) {
    mMenuItems[mActiveMenu]->setPrevColor();
  }

  if (mActiveMenu != index && index != -1) {
    mMenuItems[index]->setColor(Text::YELLOW);
  }

  mActiveMenu = index;

  return false;
}

/**
 * @brief
 *   Keyboard function
 *
 * TODO: Remove from menu
 *
 * @param key
 */
bool Menu::setActiveMenuKeyboard(const int key) {
  if (!isVisible())
    return false;

  int activeMenu = -2;

  if (key == GLFW_KEY_UP) {
    int size = mMenuItems.size() - 1;
    activeMenu =
      mActiveMenu == -1 || mActiveMenu == 0 ? size - 1 : mActiveMenu - 1;
  } else if (key == GLFW_KEY_DOWN) {
    int size   = mMenuItems.size() - 1;
    activeMenu = mActiveMenu == size || mActiveMenu == -1 ? 0 : size - 1;
  }

  if (activeMenu != -2) {
    return setActiveMenu(activeMenu);
  }

  return false;
}

/**
 * @brief
 *   Sets the new offset of the menu, setting
 *   the offset of all the items too.
 *
 * @param offset
 */
void Menu::setOffset(const vec2& offset) {
  mOffset = offset;

  for (unsigned int i = 0; i < mMenuItems.size(); i++)
    mMenuItems[i]->setOffset(offset);
}

/**
 * @brief
 *   Returns the index of the active menu item
 *
 * @return index, may be -1
 */
int Menu::getActiveMenu() const {
  return mActiveMenu;
}

/**
 * @brief
 *   Returns the Text element for the item that is active. If there
 *   is no item that is active, the return value is a nullptr
 *
 * @return
 */
const Text* Menu::getActiveMenuItem() const {
  if (mActiveMenu < 0 || mActiveMenu >= (int) mMenuItems.size())
    return nullptr;
  return mMenuItems[mActiveMenu];
}

/**
 * @brief
 *   Draws the menu and all its sub elements
 *
 * @param float
 */
void Menu::draw() {
  if (!isVisible())
    return;

  for (unsigned int i = 0; i < mMenuItems.size(); i++)
    mMenuItems[i]->draw();
}
