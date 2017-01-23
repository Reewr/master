#include "GUI.hpp"

#include "../GLSL/Program.hpp"
#include "../Input/Event.hpp"
#include "../Resource/ResourceManager.hpp"
#include "../Utils/Asset.hpp"
#include "../Utils/CFG.hpp"

Asset* GUI::mAsset = nullptr;

GUI::GUI()
    : mBoundingBox(0, 0, 0, 0)
    , mOffset(0, 0)
    , mIsVisible(false)
    , mIsClickable(false)
    , mIsAnimating(false)
    , mIsMinimized(false)
    , mHasChanged(false)
    , mIsMouseOver(false) {
  mGUIProgram = mAsset->rManager()->get<Program>("Program::GUI");
  mGUIProgram->setUniform("screenRes", mAsset->cfg()->graphics.res);
}

GUI::~GUI() {}

bool GUI::isInside(const vec2& pos) const {
  return mBoundingBox.contains(pos);
}

void GUI::draw() {}
void GUI::update(float) {}

bool GUI::isVisible() const {
  return mIsVisible;
}
bool GUI::isClickable() const {
  return mIsClickable;
}
bool GUI::isAnimating() const {
  return mIsAnimating;
}
bool GUI::isMinimized() const {
  return mIsMinimized;
}
bool GUI::isMouseOver() const {
  return mIsMouseOver;
}

bool GUI::hasChanged() const {
  return mHasChanged;
}

void GUI::input(const Input::Event& event) {
  if (!isVisible())
    return;

  if (mInputHandler)
    return mInputHandler(event);

  defaultInputHandler(event);
}

void GUI::setInputHandler(
  std::function<void(const Input::Event& event)> handler) {
  mInputHandler = handler;
}

void GUI::defaultInputHandler(const Input::Event&) {}

void GUI::isVisible(bool v) {
  mIsVisible = v;
}
void GUI::isClickable(bool c) {
  mIsClickable = c;
}
void GUI::isAnimating(bool a) {
  mIsAnimating = a;
}
void GUI::isMinimized(bool m) {
  mIsMinimized = m;
}
void GUI::hasChanged(bool c) {
  mHasChanged = c;
}
void GUI::isMouseOver(bool m) {
  mIsMouseOver = m;
}

const Rect& GUI::box() const {
  return mBoundingBox;
}
const vec2& GUI::position() const {
  return mBoundingBox.topleft;
}
const vec2& GUI::size() const {
  return mBoundingBox.size;
}
const vec2& GUI::offset() {
  return mOffset;
}

void GUI::setPosition(const vec2& position) {
  mBoundingBox.topleft = position;
}
void GUI::setSize(const vec2& size) {
  mBoundingBox.size = size;
}

void GUI::setOffset(const vec2& offset) {
  mOffset = offset;
}
