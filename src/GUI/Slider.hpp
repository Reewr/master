#pragma once

#include <string>

#include "../OpenGLHeaders.hpp"

#include "../Math/MathCD.hpp"
#include "GUI.hpp"

class Texture;
class Text;

namespace tinyxml2 {
class XMLElement;
}

class Slider : public GUI {
public:
  //! Loads textures and calls recalculatesGeometry
  Slider(const vec2&        pos,
         const float        scale   = 1,
         const std::string& valSign = "%");

  static Slider* fromXML(tinyxml2::XMLElement* element);

  //! Deletes textures
  ~Slider();

  //! Can be called to do the default actions by using
  //! setInputHandler. Is also called by default if setInputHandler
  //! is never called.
  void defaultInputHandler(const Input::Event& event);

  //! Changing the offset of all items
  void setOffset(const vec2& offset);

  float value() const;

  //! Moves the slider on the X axis
  bool moveSlider(const vec2& position);

  //! Sets the slider to a specific position based a value
  void setSlider(float s);

  //! :)
  void draw(float deltaTime);

private:
  Texture* mBackground;
  Texture* mButton;
  Text*    mInfo;

  float       mScale;
  float       mValue;
  std::string mValSign;

  Rect mButtonRect;
  vec2 mButtonOffset;
};