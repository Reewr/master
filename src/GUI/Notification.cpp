#include "Notification.hpp"

#include "../GLSL/Program.hpp"
#include "../Graphical/Font.hpp"
#include "../Graphical/Text.hpp"
#include "../Graphical/Texture.hpp"
#include "../Utils/Asset.hpp"
#include "../Utils/Utils.hpp"

Notification::Notification() {}

Notification::Notification(const Rect& rect,
                           const int   reason,
                           const bool  isMinimized) {
  mBoundingBox = rect;
  mAbout       = reason;

  mNotTex = new Texture(TEMP::getPath(TEMP::OPTSMENU));
  mIcon   = new Texture(TEMP::getPath(TEMP::POWERI));

  mText = new Text(mFont, "", mBoundingBox.topleft + 10.0, 15, Text::WHITE);
  mButtonText = new Text(mFont, "Cancel", vec2(0, 0), 15, Text::WHITE);

  mButtonText->setPosition(
    vec2(mBoundingBox.middle().x - mButtonText->box().bottomright().x / 2,
         mBoundingBox.bottomright().y - 20));
  mIconRect =
    Rect(vec2(mBoundingBox.bottomright().x - 50, mBoundingBox.topleft.y + 12.5),
         vec2(50, 50));

  addNewNotification(reason);

  mNotTex->recalculateGeometry(mBoundingBox);
  mIcon->recalculateGeometry(mIconRect);

  setOffset(vec2(-250, 0));
  mNewPos = vec2(isMinimized ? 50 : -mOffset.x, 0);
  isAnimating(true);
  mWaitTime      = 0.5;
  mAnimationTime = 0.5;
  mCurrentTime   = 0;
}

Notification::~Notification() {
  delete mText;
  delete mIcon;
  delete mButtonText;
  delete mNotTex;
}

void Notification::addNewNotification(const int reason) {
  switch (reason) {
    case Notification::LOWPOWER:
      mText->setText("Low power. Click here for info");
      mText->setLimit(vec2(150, 200));
      break;
    case Notification::IDLEROBOT:
      mText->setText(
        "Queues are building up at the charging stations. I suggest building "
        "more of them.");
      break;
  }
}

void Notification::moveWindow(const vec2& newPos) {
  isAnimating(true);
  mNewPos        = newPos;
  mAnimationTime = 0.5;
  mWaitTime      = 0;
  mCurrentTime   = 0;
}

void Notification::runAnimation(float deltaTime) {
  if (mWaitTime > 0) {
    mWaitTime -= deltaTime;
    return;
  }
  mCurrentTime += deltaTime;
  if (mAnimationTime - mCurrentTime < 0)
    deltaTime = mCurrentTime - mAnimationTime;

  vec2 movement((mNewPos.x / mAnimationTime) * deltaTime,
                (mNewPos.y / mAnimationTime) * deltaTime);

  mOffset += movement;
  setOffset(mOffset);

  if (mAnimationTime - mCurrentTime <= 0) {
    isAnimating(false);
  }
}

void Notification::showCancelSelection(const vec2& pos) {
  mButtonText->setColor(isInsideCancel(pos) ? Text::YELLOW : Text::WHITE);
}

bool Notification::isInsideCancel(const vec2& pos) const {
  return (mButtonText->box().contains(pos - mOffset));
}

bool Notification::hasWaitTime() const {
  return (mWaitTime > 0);
}

void Notification::setOffset(const vec2& offset) {
  mOffset = offset;
  mText->setOffset(offset);
  mButtonText->setOffset(offset);
}

void Notification::draw() {
  if (isAnimating())
    runAnimation(5);

  mGUIProgram->setUniform("guiOffset", mOffset);
  mNotTex->draw();
}

void Notification::drawIcon() {
  mGUIProgram->setUniform("guiOffset", mOffset);
  mIcon->draw();
}

void Notification::drawText() {
  mText->draw();
  mButtonText->draw();
}
