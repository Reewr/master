#include "Dropdown.hpp"

#include <iterator>
#include <tinyxml2.h>

#include "../GLSL/Program.hpp"
#include "../Input/Event.hpp"
#include "../Resource/ResourceManager.hpp"
#include "../Resource/Texture.hpp"
#include "../Shape/GL/Rectangle.hpp"
#include "../Utils/Asset.hpp"
#include "../Utils/str.hpp"
#include "Text.hpp"

using mmm::vec2;

/**
 * @brief
 *   Creates a dropdown that has the given options
 *   at the given position
 *
 * @param options
 *   A list of string that will be displayed in the dropdown
 *
 * @param position
 *   Dropdown position
 */
Dropdown::Dropdown(const std::vector<std::string>& options,
                   const vec2&                     position) {
  mBox                  = new GLRectangle();
  mOptionsList          = new GLRectangle();
  mIsVisible            = true;
  mIsOptionsListVisible = false;
  mMouseOption          = -1;
  mBoundingBox          = Rectangle(position, vec2(0, 25));

  // Padding on the top
  addOption("-----");

  // add the rest of the options
  for (unsigned int i = 0; i < options.size(); i++)
    addOption(options[i]);

  // add some padding to the sides
  mBoundingBox.size.x += 25;
  mActiveOption = options.size() > 1 ? 1 : 0;

  // Add sizes together, assuming each element is ~25px
  mBigBoxRect = Rectangle(mBoundingBox.topleft,
                          mBoundingBox.size + vec2(0, mOptions.size() * 25));

  mBox->change(mBoundingBox);
  mBox->setTexture(mAsset->rManager()->get<Texture>("Texture::Dropdown"));
  mOptionsList->change(mBigBoxRect);
  mOptionsList->setTexture(
    mAsset->rManager()->get<Texture>("Texture::Background"));
  setActiveOptionPosition();
}

/**
 * @brief
 *   Creates a dropdown from an XML document.
 *
 *   The xml element looks like:
 *
 *   <guidropdown x="400" y="35">
 *     <item name="Window">
 *     <item name="Borderless">
 *     <item name="Fullscreen">
 *   </guidropdown>
 *
 * @param element
 *
 * @return
 *   a newly allocated dropdown that has to be deleted manually
 */
Dropdown* Dropdown::fromXML(tinyxml2::XMLElement* element) {
  if (element == nullptr) {
    throw std::runtime_error("XMLElement is null");
  }

  vec2 position;

  // retrieve the positions of the dropdown
  if (element->QueryFloatAttribute("x", &position.x) != 0) {
    throw std::runtime_error("Float position 'x' does not exist in XMLElement");
  }

  if (element->QueryFloatAttribute("x", &position.y) != 0) {
    throw std::runtime_error("Float position 'y' does not exist in XMLElement");
  }

  std::vector<std::string> strOptions;
  tinyxml2::XMLElement*    options = element->FirstChildElement();

  for (; options != nullptr; options = options->NextSiblingElement()) {
    const char* option = options->Attribute("name");

    if (option == nullptr) {
      throw std::runtime_error("Option has no attribute 'name'");
    }

    strOptions.push_back(std::string(option));
  }

  return new Dropdown(strOptions, position);
}

Dropdown::~Dropdown() {
  delete mBox;
  delete mOptionsList;

  for (auto a : mOptions)
    delete a;

  mOptions.clear();
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
 */
void Dropdown::defaultInputHandler(const Input::Event& event) {
  if (!isVisible())
    return;

  if (event.buttonPressed(GLFW_MOUSE_BUTTON_1)) {
    if (setActiveItem(event.position())) {
      event.stopPropgation();
    }

    return;
  }

  if (!mIsOptionsListVisible)
    return;

  if (event == Input::Event::Type::MouseMovement) {
    setMouseOverItem(isInsideDropItem(event.position()));
  }
}

/**
 * @brief
 *   Adds an option to the options list by creating
 *   a new text object and setting the positions
 *   based on the number of options already created.
 *   The Dropbox's bounding box will be adjusted
 *   if the size of the text is larger than that of
 *   the dropbox
 *
 * @param text the text to show on the option
 */
void Dropdown::addOption(const std::string text) {
  float height    = (mOptions.size() + 1) * 25;
  vec2  optionPos = vec2(5, height);
  Text* option    = new Text("Font::Dejavu", text, vec2(0, 0), 15);
  vec2  size      = option->size();

  mBoundingBox.size.x = mmm::max(mBoundingBox.size.x, size.x);
  mBoundingBox.size.y = mmm::max(mBoundingBox.size.y, size.y);

  option->setPosition(mBoundingBox.topleft + optionPos);
  option->setColor(Text::WHITE);

  mOptions.push_back(option);
}

/**
 * @brief
 *   Checks whether the position is within the larger
 *   box that contains the options.
 *
 * @param position
 *   screen position
 *
 * @return
 *   true if inside
 */
bool Dropdown::isInsideOptionsList(const vec2& position) const {
  return mBigBoxRect.contains(position);
}

/**
 * @brief
 *   Checks whether a position is within one of the options
 *   that the dropdown has. Returns -1 if there are no
 *   matches or if the dropdown isnt open, else it returns
 *   the index of the element
 *
 * @param position
 *   screen position
 *
 * @return
 *   index, -1 if no match
 */
int Dropdown::isInsideDropItem(const vec2& position) const {
  // if the dropbox isnt open, we always return -1
  if (!mIsOptionsListVisible || !isInsideOptionsList(position)) {
    return -1;
  }

  // Check through the options, returning an index if
  // we find a match
  for (unsigned int i = 0; i < mOptions.size(); i++) {
    if (mOptions[i]->isInside(position)) {
      return i;
    }
  }

  // No match == -1
  return -1;
}

/**
 * @brief
 *   Sets item that should get the style of being
 *   moused over. What this does, is it sets
 *   the color of the option to yellow.
 *
 * @param index
 *   The index to use or -1 to deselect all
 */
void Dropdown::setMouseOverItem(const int index) {
  if (!mIsOptionsListVisible && index != -1)
    return;

  // if there's already one selected, set back to
  // whateever color it had.
  if (mMouseOption != -1) {
    mOptions[mMouseOption]->setPrevColor();
  }

  // set the highlight color on new index
  if (index != -1) {
    mOptions[index]->setColor(Text::YELLOW);
  }

  if (index == -1 || index <= (int) mOptions.size()) {
    mMouseOption = index;
  }
}

/**
 * @brief
 *   Sets which item should be active based on the position
 *   given. If the position is on the box itself, its opened,
 *   showing the options list.
 *
 *   If the option list is already open, the click is checked
 *   to see whether it is inside any of the given options,
 *   if so, they're selected.
 *
 * @param position
 *   screen position
 */
bool Dropdown::setActiveItem(const vec2& position) {
  // Close the dropdown if inside the button and its open,
  // else open it
  if (isInside(position)) {
    mIsOptionsListVisible = !mIsOptionsListVisible;
    setMouseOverItem(-1);
    return true;
  }

  // If the big box isnt visible, there's nothing to click
  if (!mIsOptionsListVisible) {
    return false;
  }

  // If the click is outside the large box, we close
  // dropdown
  if (!isInsideOptionsList(position)) {
    mIsOptionsListVisible = false;
    setMouseOverItem(-1);
    return false;
  }

  // Go through all the elements, find the match (if any)
  // and ignore index 0 as that is a padding element
  for (unsigned int i = 0; i < mOptions.size(); i++) {
    if (mOptions[i]->isInside(position)) {
      // Ignore the padding element
      if (i == 0)
        return true;

      mActiveOption         = i;
      mIsOptionsListVisible = false;
      setActiveOptionPosition();
      hasChanged(true);
      setMouseOverItem(-1);
      return true;
    }
  }

  return false;
}

/**
 * @brief
 *   Unlike setActiveItem(const vec2& position), this sets
 *   the active option directly when it finds an option
 *   that matches the given text.
 *
 * @param text
 */
void Dropdown::setActiveItem(const std::string text) {
  std::string loweredText = str::toLower(text);

  for (unsigned int i = 0; i < mOptions.size(); i++) {
    if (str::toLower(mOptions[i]->getText()) == loweredText) {
      mActiveOption = i;
      setActiveOptionPosition();
    }
  }
}

/**
 * @brief
 *   Sets the active item by index, which is legal
 *   as long as the index is larger than 0 and smaller
 *   than max index
 *
 * @param index
 */
void Dropdown::setActiveItemIndex(const int index) {
  if (index < 0 || index > (int) mOptions.size())
    return;

  mActiveOption = index;
  setActiveOptionPosition();
}

/**
 * @brief
 *   Returns the text of the currently active item.
 *   If there are no elements in the list this may be
 *   a line of hypens, otherwise it is by default
 *   the first option unless changed by other functions.
 *
 * @return the text of the option
 */
std::string Dropdown::activeItemText() const {
  return mOptions[mActiveOption]->getText();
}

/**
 * @brief
 *   Returns the index of the currently active
 *   option.
 *
 * @return index, always positive
 */
int Dropdown::activeItemIndex() const {
  return mActiveOption;
}

/**
 * @brief
 *   Sets the active options position.
 */
void Dropdown::setActiveOptionPosition() {
  if (mActiveOption >= (int) mOptions.size())
    return;

  Text* selected = mOptions[mActiveOption];
  int   y        = mBoundingBox.topleft.y + mOffset.y - selected->position().y;
  mActiveOptionPosition = vec2(selected->offset().x, y);
}

/**
 * @brief
 *   Sets the position of the dropdown element.
 *   Also sets the position of all its elements.
 *
 * @param position
 */
void Dropdown::setPosition(const vec2& position) {
  for (auto option : mOptions) {
    vec2 actualPosition = mBoundingBox.topleft - option->position();
    option->setPosition(position + actualPosition);
  }

  mBoundingBox.topleft = position;
}

/**
 * @brief
 *   Sets the offset of the element. Also
 *   adjusts the sub elements such as optionListBox
 *   and its elements.
 *
 * @param offset
 */
void Dropdown::setOffset(const vec2& offset) {
  mOffset = offset;

  for (unsigned int i = 0; i < mOptions.size(); i++)
    mOptions[i]->setOffset(offset);

  setActiveOptionPosition();
}

/**
 * @brief
 *   Draws the dropdown element if it is visible. Also
 *   draws the options and the optionList if they
 *   are visible
 *
 * @param float delta time, not used in this case
 */
void Dropdown::draw() {
  if (!isVisible())
    return;

  mGUIProgram->bind();
  mGUIProgram->setUniform("guiOffset", mOffset);
  mBox->draw();

  // if optionList is visible, draw em all
  if (mIsOptionsListVisible) {
    mOptionsList->draw();
    for (auto& text : mOptions)
      text->draw();

    return;
  }

  // since option list isnt visible, we just draw
  // the active one, but we have to move it temporarily
  // to where the dropdown button is
  vec2 tempOffset = mOptions[mActiveOption]->offset();
  mOptions[mActiveOption]->setOffset(mActiveOptionPosition);
  mOptions[mActiveOption]->draw();
  mOptions[mActiveOption]->setOffset(tempOffset);
}
