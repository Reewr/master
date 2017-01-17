#pragma once

#include "../Math/MathCD.hpp"
#include <functional>

class Font;
class Program;
class CFG;

namespace Input {
  class Event;
}

//! Fully Virtual class used by all GUI elements.
class GUI {
public:
  virtual ~GUI();
  virtual void draw();
  virtual void update(float deltaTime);

  virtual bool isInside(const vec2& pos) const;

  //! Check whether the GUI element is visible or not
  virtual bool isVisible() const;

  //! Check whether the GUI element is clickable or not
  virtual bool isClickable() const;

  //! Check whether the GUI element is animating
  virtual bool isAnimating() const;

  //! Check whether the GUI element is minimized or not
  virtual bool isMinimized() const;

  //! Check whether the GUI element is being mousedover
  virtual bool isMouseOver() const;

  virtual bool hasChanged() const;
  virtual void hasChanged(bool c);

  virtual void isVisible(bool v);
  virtual void isClickable(bool c);
  virtual void isMinimized(bool m);
  virtual void isAnimating(bool a);
  virtual void isMouseOver(bool m);

  virtual void input(const Input::Event& event);

  virtual void
  setInputHandler(std::function<void(const Input::Event& event)> handler);

  virtual void defaultInputHandler(const Input::Event& event);

  virtual const Rect& box() const;
  virtual const vec2& position() const;
  virtual const vec2& size() const;
  virtual const vec2& offset();

  virtual void setPosition(const vec2& position);
  virtual void setSize(const vec2& size);
  virtual void setOffset(const vec2& offset);

  static void init(CFG* cfg);
  static void deinit();

protected:
  GUI();

  Rect mBoundingBox;
  Rect mMouseoverBox;

  vec2 mOffset;
  bool mIsVisible;
  bool mIsClickable;
  bool mIsAnimating;
  bool mIsMinimized;
  bool mHasChanged;
  bool mIsMouseOver;

  std::function<void(const Input::Event& event)> mInputHandler;

  static CFG*     mCFG;
  static Font*    mFont;
  static Program* mGUIProgram;
};
