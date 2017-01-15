#pragma once

#include <string>
#include <tinyxml2.h>
#include <typeinfo>

#include "../Math/Math.hpp"

class Window;

namespace Import {

class UILoader {
public:
  UILoader();
  bool loadXML(std::string filepath);
  bool loadXMLSettings(std::string filepath, std::string settings, Window* win);
  void loadSettings(std::string setting, Window* win);

private:
  void getAttrib(tinyxml2::XMLElement* e, const char* a, int* t);
  void getAttrib(tinyxml2::XMLElement* e, const char* a, float* t);
  void
  getAttrib(tinyxml2::XMLElement* e, const char* a, const char* b, vec2& v);
  std::string getAttrib(tinyxml2::XMLElement* e, const char* a);

  void handleWindowElement(tinyxml2::XMLElement* winElement,
                           Window*               win,
                           bool                  isOnlyWin = false);
  void handleMenuElement(tinyxml2::XMLElement* elem, Window* w);
  void handleMenuItemElement(tinyxml2::XMLElement* elem, Window* w);
  void handleDropdownElement(tinyxml2::XMLElement* e, Window* w);
  void handleSliderElement(tinyxml2::XMLElement* e, Window* w);
  void handleCheckboxElement(tinyxml2::XMLElement* e, Window* w);
  void handleInputboxElement(tinyxml2::XMLElement* e, Window* w);

  enum {
    FILEERROR,
    LOADSETTERROR,
    INVSCREEN,
    ATTRIBINTFAIL,
    ATTRIBFLOFAIL,
    ATTRIBSTRFAIL
  };
  std::string getError(int error);
  tinyxml2::XMLDocument xml;
  int                   errorID;
};
}