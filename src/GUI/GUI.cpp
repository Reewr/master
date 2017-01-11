#include "GUI.hpp"

#include "../GLSL/Program.hpp"
#include "../Graphical/Font.hpp"
#include "../Math/Math.hpp"
#include "../Utils/CFG.hpp"
#include "../Utils/Utils.hpp"

CFG*     GUI::mCFG        = NULL;
Program* GUI::mGUIProgram = NULL;
Font*    GUI::mFont       = NULL;

GUI::GUI()
    : mBoundingBox(0, 0, 0, 0)
    , mOffset(0, 0)
    , mIsVisible(false)
    , mIsClickable(false)
    , mIsAnimating(false)
    , mIsMinimized(false)
    , mHasChanged(false)
    , mIsMouseOver(false) {}

GUI::~GUI() {}

bool GUI::isInside(const vec2& pos) const {
  return mBoundingBox.contains(pos);
}

void GUI::draw() {}
void GUI::draw(float) {}

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

void GUI::init(CFG* c) {
  mCFG        = c;
  mFont       = new Font(TEMP::getPath(TEMP::FONT));
  mGUIProgram = new Program("shaders/GUI/GUI.vsfs", 0);
  mGUIProgram->bindAttribs({ "position", "texcoord" }, { 0, 1 });
  mGUIProgram->link();
  mGUIProgram->setUniform("screenRes", mCFG->graphics.res, "guiOffset", vec2());
  mGUIProgram->setUniform("isText", 0);
  mGUIProgram->setUniform("guiColor", vec3());
}

void GUI::deinit() {
  delete mGUIProgram;
  delete mFont;
}
