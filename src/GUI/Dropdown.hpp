#ifndef GUI_DROPDOWN_HPP
#define GUI_DROPDOWN_HPP

#include <string>
#include <vector>

#include "GUI.hpp"
#include "../Math/MathCD.hpp"

class Texture;
class Text;

namespace tinyxml2 {
  class XMLElement;
}

class Dropdown : public GUI {
public:

  //! Creates a dropbox with predetermined textures for dropboxes.
  Dropdown(const std::vector<std::string>& options, const vec2& position);

  //!
  ~Dropdown();

  static Dropdown* fromXML(tinyxml2::XMLElement* element);

  //! Returns true if inside the big box with all the options
  bool isInsideOptionsList(const vec2& position) const;

  //! Returns the index of which dropboxItem the position is inside
  int isInsideDropItem(const vec2& position) const;

  //! Turns the color of text to yellow.
  void setMouseOverItem(int i);

  //! Sets active item based on position
  void setActiveItem(const vec2& position);

  //! Sets active item based on string
  void setActiveItem(std::string text);

  //! Sets the active itme by index
  void setActiveItemIndex(int index);

  //! Returns the activeitem as a string
  std::string activeItemText() const;

  //! returns the activeItem index
  int activeItemIndex() const;

  //! Sets the position of the dropdown. Also moves
  //! the other elements with it.
  void setPosition(const vec2& position);

  //! Sets the offset for the dropbox as well as any elements in it.
  void setOffset(const vec2& offset);

  //! Draws
  void draw(const float deltaTime);

private:
  //! Add an options to the dropbox
  void addOption(std::string text);

  //! sets the active options position,
  //! used by the drawing function and set
  //! whenever the active option changes
  void setActiveOptionPosition();

  Texture* mBox;
  Texture* mOptionsList;
  vec2 mActiveOptionPosition;

  std::vector<Text*> mOptions;

  int mActiveOption;
  int mMouseOption;

  bool mIsOptionsListVisible;

  Rect mBigBoxRect;
};

#endif