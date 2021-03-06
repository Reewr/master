#include "Window.hpp"

#include <algorithm>
#include <tinyxml2.h>

#include "Checkbox.hpp"
#include "Dropdown.hpp"
#include "Inputbox.hpp"
#include "Slider.hpp"
#include "Text.hpp"

#include "../GLSL/Program.hpp"
#include "../Input/Event.hpp"
#include "../Resource/ResourceManager.hpp"
#include "../Resource/Texture.hpp"
#include "../Shape/GL/Rectangle.hpp"
#include "../Utils/Asset.hpp"

using mmm::vec2;

Window::Window() : Logging::Log("Window") {
  mBackground = nullptr;
  mTitle      = nullptr;

  mBoundingBox = Rectangle({ 0, 0 }, { 0, 0 });
}

Window::Window(std::string texture, Rectangle r) : Logging::Log("Window") {
  mBackground = nullptr;
  mTitle      = nullptr;

  mBoundingBox = r;

  if (texture != "NONE") {
    mBackground = new GLRectangle(mBoundingBox);
    mBackground->setTexture(mAsset->rManager()->get<Texture>(texture));
  }
}

void handleGUIElement(Window* winodw, tinyxml2::XMLElement* element);

void handleMenuElement(Window* window, tinyxml2::XMLElement* element) {
  tinyxml2::XMLElement* itemElement = element->FirstChildElement();

  for (; itemElement != nullptr;
       itemElement = itemElement->NextSiblingElement()) {
    tinyxml2::XMLElement* child = itemElement->FirstChildElement();

    for (; child != nullptr; child = child->NextSiblingElement()) {
      handleGUIElement(window, child);
    }
  }
}

void handleGUIElement(Window* window, tinyxml2::XMLElement* element) {
  const char* cStrType = element->Name();
  const char* cStrName = element->Attribute("name");
  std::string type     = std::string(cStrType);

  if (cStrName == nullptr) {
    throw std::runtime_error("XMLElement: '" + type +
                             "' has no name attribute");
  }

  std::string name   = std::string(cStrName);
  GUI*        widget = nullptr;

  if (type == "guidropdown") {
    widget = Dropdown::fromXML(element);
  } else if (type == "guicheckbox") {
    widget = Checkbox::fromXML(element);
  } else if (type == "guiinputbox") {
    widget = Inputbox::fromXML(element);
  } else if (type == "guislider") {
    widget = Slider::fromXML(element);
  } else if (type == "guimenu") {
    widget = Menu::fromXML(element);
    handleMenuElement(window, element);
  } else if (type == "window") {
    widget = Window::fromXML(element);
  } else {
    throw std::runtime_error("XMLElement: '" + type + "' is not supported");
  }

  window->add(name, widget);
}

Window* Window::fromXML(tinyxml2::XMLElement* element) {
  if (element == nullptr) {
    throw std::runtime_error("XMLElement is null");
  }

  const char* cStrTexture = element->Attribute("texture");
  const char* cStrName    = element->Attribute("name");
  const char* cStrTitle   = element->Attribute("title");

  if (cStrName == nullptr) {
    throw std::runtime_error("XMLElement has no attribute 'name'");
  }

  std::string texture =
    std::string(cStrTexture == nullptr ? "NONE" : cStrTexture);
  std::string name  = std::string(cStrName);
  std::string title = std::string(cStrTitle == nullptr ? "" : cStrTitle);

  Window* window = new Window();

  // add title if it exists
  if (title != "") {
    window->addTitle(title);
  }

  tinyxml2::XMLElement* childElement = element->FirstChildElement();

  if (!childElement) {
    return window;
  }

  for (; childElement != nullptr;
       childElement = childElement->NextSiblingElement()) {
    handleGUIElement(window, childElement);
  }

  return window;
}

Window::~Window() {
  if (mBackground != nullptr)
    delete mBackground;

  if (mTitle != nullptr)
    delete mTitle;

  for (auto m : mMenues)
    delete m.second;

  for (auto s : mSliders)
    delete s.second;

  for (auto w : mWindows)
    delete w.second;

  for (auto d : mDropdowns)
    delete d.second;

  for (auto c : mCheckboxes)
    delete c.second;

  for (auto i : mInputboxes)
    delete i.second;

  mMenues.clear();
  mSliders.clear();
  mWindows.clear();
  mDropdowns.clear();
  mCheckboxes.clear();
  mInputboxes.clear();
}

int Window::handleKeyInput(int, int) {
  return -1;
}
int Window::handleMouseButton(int, int) {
  return -1;
}
int Window::handleAction() {
  return -1;
}

void Window::addTitle(std::string s) {
  if (mTitle != nullptr)
    delete mTitle;

  mTitle = new Text("Font::Dejavu", s, vec2(0, 0), 40);
  mTitle->setPosition(vec2(mBoundingBox.middle().x - mTitle->box().middle().x,
                           mBoundingBox.topleft.y + 20));
  mTitle->setColor(mBackground != nullptr ? Text::WHITE : Text::BLACK);
}

void Window::addWindow(std::string name, Rectangle r, std::string tex) {
  if (mWindows.count(name))
    delete mWindows[name];

  r              = Rectangle(mBoundingBox.topleft + r.topleft, r.size);
  mWindows[name] = new Window(tex, r);
}

void Window::add(std::string name, GUI* widget) {
  if (widget == nullptr) {
    throw std::runtime_error("GUI Widget is null");
  }

  widget->setPosition(mBoundingBox.topleft + widget->position());

  // this is very temporary as I just want to get the XML
  // importing working before I start messing with the
  // other stuff in this class.
  //
  // Ideally, the system would have 1 map with GUI-types
  if (Menu* menu = dynamic_cast<Menu*>(widget)) {
    if (mMenues.count(name))
      delete mMenues[name];

    mMenues[name] = menu;
  } else if (Slider* slider = dynamic_cast<Slider*>(widget)) {
    if (mSliders.count(name))
      delete mSliders[name];

    mSliders[name] = slider;
  } else if (Dropdown* dropdown = dynamic_cast<Dropdown*>(widget)) {
    if (mDropdowns.count(name))
      delete mDropdowns[name];

    mDropdowns[name] = dropdown;
    sortDropdowns();
  } else if (Checkbox* checkbox = dynamic_cast<Checkbox*>(widget)) {
    if (mCheckboxes.count(name))
      delete mCheckboxes[name];

    mCheckboxes[name] = checkbox;
  } else if (Inputbox* inputbox = dynamic_cast<Inputbox*>(widget)) {
    if (mInputboxes.count(name))
      delete mInputboxes[name];

    mInputboxes[name] = inputbox;
  } else if (Window* window = dynamic_cast<Window*>(widget)) {
    if (mWindows.count(name))
      delete mWindows[name];

    mWindows[name] = window;
  } else {
    throw std::runtime_error("No implementation for element");
  }
}

void Window::addMenu(std::string                     name,
                     const std::vector<std::string>& names,
                     const vec2&                     pos,
                     const Menu::MenuSettings&       m) {
  if (mMenues.count(name))
    delete mMenues[name];
  vec2 newPos   = mBoundingBox.topleft + pos;
  mMenues[name] = new Menu(names, newPos, m);
}

void Window::addMenuItem(std::string               name,
                         std::string               text,
                         const vec2&               pos,
                         const Menu::MenuSettings& m) {

  if (!mMenues.count(name))
    throw std::runtime_error("There is no menu with name: '" + name + "'");

  vec2 newPos = mBoundingBox.topleft + pos;
  mMenues[name]->addMenuItem(text, newPos, m);
}

void Window::addSlider(std::string name, vec2 pos, float scale) {
  if (mSliders.count(name))
    delete mSliders[name];
  pos            = mBoundingBox.topleft + pos;
  mSliders[name] = new Slider(pos, scale);
}

void Window::addDropdown(std::string              name,
                         std::vector<std::string> options,
                         vec2                     pos) {
  if (mDropdowns.count(name))
    delete mDropdowns[name];

  pos              = mBoundingBox.topleft + pos;
  mDropdowns[name] = new Dropdown(options, pos);
  sortDropdowns();
}

void Window::addCheckbox(std::string name, vec2 pos) {
  if (mCheckboxes.count(name))
    delete mCheckboxes[name];

  pos               = mBoundingBox.topleft + pos;
  mCheckboxes[name] = new Checkbox(pos);
}

void Window::addInputbox(std::string name, Rectangle r, std::string text) {
  if (mInputboxes.count(name))
    delete mInputboxes[name];

  r                 = Rectangle(mBoundingBox.topleft + r.topleft, r.size);
  mInputboxes[name] = new Inputbox(r, text);
}

void Window::setOffset(const vec2& offset) {
  if (mTitle != nullptr)
    mTitle->setOffset(offset);

  for (auto m : mMenues)
    m.second->setOffset(offset);

  for (auto w : mWindows)
    w.second->setOffset(offset);

  for (auto s : mSliders)
    s.second->setOffset(offset);

  for (auto d : mDropdowns)
    d.second->setOffset(offset);

  for (auto c : mCheckboxes)
    c.second->setOffset(offset);

  for (auto i : mInputboxes)
    i.second->setOffset(offset);

  mOffset = offset;
}

void Window::setActiveMenuItem(std::string name, vec2 pos) {
  if (!mMenues.count(name))
    return;
  mMenues[name]->setActiveMenu(mMenues[name]->isInsideMenuElement(pos));
}

Menu* Window::menu(std::string name) {
  if (mMenues.count(name))
    return mMenues[name];
  return nullptr;
}

Slider* Window::slider(std::string name) {
  if (mSliders.count(name))
    return mSliders[name];
  return nullptr;
}

Dropdown* Window::dropdown(std::string name) {
  if (mDropdowns.count(name))
    return mDropdowns[name];
  return nullptr;
}

Checkbox* Window::checkbox(std::string name) {
  if (mCheckboxes.count(name))
    return mCheckboxes[name];
  return nullptr;
}

Inputbox* Window::inputbox(std::string name) {
  if (mInputboxes.count(name))
    return mInputboxes[name];
  return nullptr;
}

Window* Window::window(std::string name) {
  if (mWindows.count(name))
    return mWindows[name];
  return nullptr;
}

std::map<std::string, Menu*> Window::menues() {
  return mMenues;
}

std::map<std::string, Slider*> Window::sliders() {
  return mSliders;
}

std::map<std::string, Dropdown*> Window::dropdowns() {
  return mDropdowns;
}

std::map<std::string, Checkbox*> Window::checkboxes() {
  return mCheckboxes;
}

std::map<std::string, Inputbox*> Window::inputboxes() {
  return mInputboxes;
}

std::map<std::string, Window*> Window::windows() {
  return mWindows;
}

void Window::hasChanged(bool c) {
  for (auto x : mDropdowns)
    x.second->hasChanged(c);

  for (auto x : mSliders)
    x.second->hasChanged(c);

  for (auto x : mCheckboxes)
    x.second->hasChanged(c);

  for (auto x : mInputboxes)
    x.second->hasChanged(c);

  for (auto x : mMenues)
    x.second->hasChanged(c);

  for (auto x : mWindows)
    x.second->hasChanged(c);
}

bool Window::hasChanged() const {
  for (auto x : mDropdowns) {
    if (x.second->hasChanged())
      return true;
  }

  for (auto x : mSliders) {
    if (x.second->hasChanged())
      return true;
  }

  for (auto x : mCheckboxes) {
    if (x.second->hasChanged())
      return true;
  }

  for (auto x : mInputboxes) {
    if (x.second->hasChanged())
      return true;
  }

  for (auto x : mMenues) {
    if (x.second->hasChanged())
      return true;
  }

  for (auto x : mWindows) {
    if (x.second->hasChanged())
      return true;
  }

  return false;
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
void Window::defaultInputHandler(const Input::Event& event) {
  for (auto x : mDropdowns) {
    x.second->input(event);

    if (event.hasBeenHandled())
      return;
  }

  for (auto x : mSliders) {
    x.second->input(event);

    if (event.hasBeenHandled())
      return;
  }

  for (auto x : mCheckboxes) {
    x.second->input(event);

    if (event.hasBeenHandled())
      return;
  }

  for (auto x : mInputboxes) {
    x.second->input(event);

    if (event.hasBeenHandled())
      return;
  }

  for (auto x : mMenues) {
    x.second->input(event);

    if (event.hasBeenHandled())
      return;
  }

  for (auto x : mWindows) {
    x.second->input(event);

    if (event.hasBeenHandled())
      return;
  }
}

void Window::sortDropdowns() {
  mDrawDropdowns.clear();

  for (auto a : mDropdowns)
    mDrawDropdowns.push_back(a.second);

  std::sort(mDrawDropdowns.begin(),
            mDrawDropdowns.end(),
            [](Dropdown* a, Dropdown* b) {
              return a->box().topleft.y > b->box().topleft.y;
            });
}

void Window::draw() {
  if (!isVisible())
    return;

  if (mBackground != nullptr) {
    mGUIProgram->bind();
    mGUIProgram->setUniform("guiOffset", mOffset);
    mBackground->draw();
  }

  for (auto m : mMenues)
    m.second->draw();

  for (auto s : mSliders)
    s.second->draw();

  for (auto d : mDrawDropdowns)
    d->draw();

  for (auto c : mCheckboxes)
    c.second->draw();

  for (auto i : mInputboxes)
    i.second->draw();

  for (auto w : mWindows)
    w.second->draw();

  if (mTitle != nullptr)
    mTitle->draw();
}
