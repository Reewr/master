#include "UILoader.hpp"

#include "../GUI/Menu.hpp"
#include "../GUI/Window.hpp"

Import::UILoader::UILoader() {
  errorID = 0;
}

/**
 * @brief
 *   Loads an XML file into memory, throwing a runtime_error
 *   if it fails to load the file.
 *
 * @param filepath
 *
 * @return
 */
bool Import::UILoader::loadXML(std::string filepath) {
  xml.LoadFile(filepath.c_str());
  errorID = xml.ErrorID();

  if (errorID != 0) {
    xml.PrintError();
    throw std::runtime_error(getError(FILEERROR) + " " + filepath);
  }

  return true;
}

/**
 * @brief
 *   Loads XML settings from an XML file, choosing
 *   a specific setting and applying them to the window given.
 *
 * @param filepath
 * @param settings
 * @param win
 *
 * @return
 */
bool Import::UILoader::loadXMLSettings(std::string filepath,
                                       std::string settings,
                                       Window*     win) {
  if (!loadXML(filepath))
    return false;
  loadSettings(settings, win);
  return true;
}

void Import::UILoader::getAttrib(tinyxml2::XMLElement* e,
                                 const char*           a,
                                 int*                  t) {
  int er = e->QueryIntAttribute(a, t);
  if (er != 0) {
    xml.PrintError();
    throw std::runtime_error(getError(ATTRIBINTFAIL) + " " + a);
  }
}

void Import::UILoader::getAttrib(tinyxml2::XMLElement* e,
                                 const char*           a,
                                 float*                t) {
  int er = e->QueryFloatAttribute(a, t);
  if (er != 0) {
    xml.PrintError();
    throw std::runtime_error(getError(ATTRIBFLOFAIL) + " " + a);
  }
}

void Import::UILoader::getAttrib(tinyxml2::XMLElement* e,
                                 const char*           a,
                                 const char*           b,
                                 mmm::vec2&            v) {
  getAttrib(e, a, &v.x);
  getAttrib(e, b, &v.y);
}

std::string Import::UILoader::getAttrib(tinyxml2::XMLElement* e,
                                        const char*           a) {
  const char* b = e->Attribute(a);
  if (!b) {
    xml.PrintError();
    throw std::runtime_error(getError(ATTRIBSTRFAIL) + " " + a);
  } else
    return std::string(b);
}

void Import::UILoader::loadSettings(std::string setting, Window* win) {
  if (errorID != 0) {
    throw std::runtime_error(getError(LOADSETTERROR) + " " + setting);
    return;
  }

  tinyxml2::XMLElement* scElem = xml.RootElement();

  if (scElem == NULL) {
    throw std::runtime_error(getError(INVSCREEN) + " " + setting);
  }

  scElem = scElem->FirstChildElement();
  for (; scElem != NULL; scElem = scElem->NextSiblingElement()) {
    std::string sName = std::string(scElem->Attribute("name"));
    std::string title = std::string(scElem->Attribute("title"));

    if (sName == setting) {
      win->addTitle(title);

      if (scElem->FirstChildElement()->Name() != std::string("window"))
        handleWindowElement(scElem, win, true);
      else {
        tinyxml2::XMLElement* winElem = scElem->FirstChildElement();
        for (; winElem != NULL; winElem = winElem->NextSiblingElement())
          handleWindowElement(winElem, win);
      }
    }
  }
}

void Import::UILoader::handleWindowElement(tinyxml2::XMLElement* winElem,
                                           Window*               win,
                                           bool                  isOnlyWin) {
  tinyxml2::XMLElement* menuElem = winElem->FirstChildElement();

  for (; menuElem != NULL; menuElem = menuElem->NextSiblingElement()) {
    Menu::MenuSettings ms;
    mmm::vec2          pos;
    std::string        winName  = std::string(winElem->Attribute("name"));
    std::string        menuName = std::string(menuElem->Attribute("name"));
    getAttrib(menuElem, "size", &ms.size);
    getAttrib(menuElem, "offset", &ms.offset);
    getAttrib(menuElem, "ori", &ms.ori);
    getAttrib(menuElem, "color", &ms.color);
    getAttrib(menuElem, "x", "y", pos);

    if (isOnlyWin) {
      win->addMenu(menuName, {}, pos, ms);
      handleMenuElement(menuElem, win);
    } else {
      win->window(winName)->addMenu(menuName, {}, pos, ms);
      handleMenuElement(menuElem, win->window(winName));
    }
  }
}

void Import::UILoader::handleMenuElement(tinyxml2::XMLElement* elem,
                                         Window*               w) {
  tinyxml2::XMLElement* itemElem = elem->FirstChildElement();
  for (; itemElem != NULL; itemElem = itemElem->NextSiblingElement()) {
    Menu::MenuSettings ms;
    mmm::vec2          pos;
    std::string        mName =
      std::string(itemElem->Parent()->ToElement()->Attribute("name"));
    std::string text = std::string(itemElem->Attribute("name"));
    getAttrib(itemElem, "size", &ms.size);
    getAttrib(itemElem, "x", "y", pos);

    handleMenuItemElement(itemElem, w);
    w->addMenuItem(mName, text, pos, ms);
  }
}

void Import::UILoader::handleMenuItemElement(tinyxml2::XMLElement* elem,
                                             Window*               w) {
  tinyxml2::XMLElement* i = elem->FirstChildElement();
  for (; i != NULL; i = i->NextSiblingElement()) {
    if (i->Name() == std::string("guidropdown"))
      handleDropdownElement(i, w);
    else if (i->Name() == std::string("guicheckbox"))
      handleCheckboxElement(i, w);
    else if (i->Name() == std::string("guiinputbox"))
      handleInputboxElement(i, w);
    else if (i->Name() == std::string("guislider"))
      handleSliderElement(i, w);
  }
}

void Import::UILoader::handleDropdownElement(tinyxml2::XMLElement* e,
                                             Window*               w) {
  mmm::vec2                pos;
  std::vector<std::string> opts;
  std::string              name = std::string(e->Attribute("name"));
  getAttrib(e, "x", "y", pos);
  tinyxml2::XMLElement* di = e->FirstChildElement();
  for (; di != NULL; di = di->NextSiblingElement())
    opts.push_back(di->Attribute("name"));
  w->addDropdown(name, opts, pos);
}

void Import::UILoader::handleSliderElement(tinyxml2::XMLElement* e, Window* w) {
  std::string name = std::string(e->Attribute("name"));
  mmm::vec2   pos;
  float       scale = 0.25;
  getAttrib(e, "x", "y", pos);
  getAttrib(e, "scale", &scale);
  w->addSlider(name, pos, scale);
}

void Import::UILoader::handleCheckboxElement(tinyxml2::XMLElement* e,
                                             Window*               w) {
  std::string name = std::string(e->Attribute("name"));
  mmm::vec2   pos;
  getAttrib(e, "x", "y", pos);
  w->addCheckbox(name, pos);
}

void Import::UILoader::handleInputboxElement(tinyxml2::XMLElement* e,
                                             Window*               w) {
  std::string name = std::string(e->Attribute("name"));
  std::string text = std::string(e->Attribute("text"));
  Rectangle   r;
  getAttrib(e, "x1", "y1", r.topleft);
  getAttrib(e, "x2", "y2", r.size);
  w->addInputbox(name, r, text);
}

std::string Import::UILoader::getError(int error) {
  switch (error) {
    case FILEERROR:
      return "Failed at loading UI XML file: ";
    case LOADSETTERROR:
      return "Tried accessing invalid XML file: ";
    case INVSCREEN:
      return "Invalid setting given to UILoader::loadSetting: ";
    case ATTRIBINTFAIL:
      return "QueryIntAttribute at Element: ";
    case ATTRIBFLOFAIL:
      return "QueryFloatAttribute at Element: ";
    case ATTRIBSTRFAIL:
      return "Atttribute at Element: ";
  }
  return "NO ERROR CODE";
}
