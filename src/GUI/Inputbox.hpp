#pragma once

#include <string>

#include "../Log.hpp"
#include "GUI.hpp"

class Text;

namespace tinyxml2 {
  class XMLElement;
}

namespace Input {
  class Event;
}

class GLRectangle;

class Inputbox : public GUI, public Logging::Log {
public:
  //! Default and only constructor
  Inputbox(const Rectangle& r, const std::string text = "NOT SET");

  //! Load from XML
  static Inputbox* fromXML(tinyxml2::XMLElement* element);

  ~Inputbox();

  //! Returns the current text that is on the input box
  std::string text() const;

  //! Can be called to do the default actions by using
  //! setInputHandler. Is also called by default if setInputHandler
  //! is never called.
  void defaultInputHandler(const Input::Event& event);

  //! Changes the text if the input box is visible. if forcechange is true,
  //!  changes the text.
  bool changeText(const std::string w, const bool forceChange = false);

  //! Show the inputbox if the click is within the smaller inputbox
  //! True if the click was inside
  bool showInputbox(const mmm::vec2& position);

  //! Sets the offset and all the elements in Inputbox
  void setOffset(const mmm::vec2& of);

  //! Draws input box
  void draw();

private:
  GLRectangle* mTextBox;
  GLRectangle* mInputBox;
  Text*        mText;
  Text*        mInputBoxText;

  Rectangle mInputBoxRect;
  bool      mInputIsVisible;
};
