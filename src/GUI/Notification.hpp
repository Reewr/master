#ifndef GUI_NOTIFICATION_HPP
#define GUI_NOTIFICATION_HPP

#include <vector>

#include "GUI.hpp"

class Text;
class Font;
class Texture;

class Notification : public GUI {
public:

  //! Types of Notifications
  enum {
    LOWPOWER,
    IDLEROBOT
  };

  //! Default constructor
  Notification();

  //! Constructor where rect = position and size and about = type of notification
  Notification(const Rect& rect, const int about, const bool isMinimized);

  //! Deletes glBuffer
  ~Notification();

  void addNewNotification(const int reason);

  //! Draws the notification and deals with animations.
  void draw(float deltaTime);
  void drawIcon();
  void drawText();

  void setOffset(const vec2& of);
  void moveWindow(const vec2& newPos);
  void runAnimation(float deltaTime);
  void showCancelSelection(const vec2& pos);

  bool isInsideCancel(const vec2& pos) const;
  bool hasWaitTime() const;

private:
  int mAbout;

  float mAnimationTime;
  float mCurrentTime;
  float mWaitTime;
  float mCloseTimer;

  Rect mIconRect;

  Texture* mNotTex;
  Texture* mIcon;
  Text* mButtonText;
  Text* mText;

  vec2 mNewPos;
};

#endif