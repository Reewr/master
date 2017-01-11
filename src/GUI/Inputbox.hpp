#ifndef GUI_INPUTBOX_HPP
#define GUI_INPUTBOX_HPP

#include <string>

#include "GUI.hpp"

class Texture;
class Text;

namespace tinyxml2 {
class XMLElement;
}

class Inputbox : public GUI {
public:
  //! Default and only constructor
  Inputbox(const Rect& r, const std::string text = "NOT SET");

  //! Load from XML
  static Inputbox* fromXML(tinyxml2::XMLElement* element);

  ~Inputbox();

  //! Returns the current text that is on the input box
  std::string text() const;

  //! Changes the text if the input box is visible. if forcechange is true,
  //!  changes the text.
  bool changeText(const std::string w, const bool forceChange = false);

  //! Show the inputbox if the click is within the smaller inputbox
  void showInputbox(const vec2& position);

  //! Sets the offset and all the elements in Inputbox
  void setOffset(const vec2& of);

  //! Draws input box
  void draw(float);

private:
  Texture* mTextBox;
  Texture* mInputBox;
  Text*    mText;
  Text*    mInputBoxText;

  Rect mInputBoxRect;
  bool mInputIsVisible;
};

#endif