#pragma once

#include "../GLSL/Program.hpp"
#include "../Shape/Rectangle.hpp"
#include "../Utils/Asset.hpp"
#include <functional>
#include <memory>

class Asset;

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

  //! This can be used, if the GUI element obeys it, to
  //! check if anything has changed. For instance, if the
  //! user has set the element to a different dropdown
  virtual bool hasChanged() const;

  // Setter functions
  virtual void hasChanged(bool c);
  virtual void isVisible(bool v);
  virtual void isClickable(bool c);
  virtual void isMinimized(bool m);
  virtual void isAnimating(bool a);
  virtual void isMouseOver(bool m);

  // The input handler itself. This is called by the active state
  virtual void input(const Input::Event& event);

  // It is often useful to customize the handler of a GUI element, this
  // allows that.
  virtual void
  setInputHandler(std::function<void(const Input::Event& event)> handler);

  virtual void defaultInputHandler(const Input::Event& event);

  virtual const Rectangle& box() const;
  virtual const vec2& position() const;
  virtual const vec2& size() const;
  virtual const vec2& offset();

  virtual void setPosition(const vec2& position);
  virtual void setSize(const vec2& size);
  virtual void setOffset(const vec2& offset);

  static Asset* mAsset;

protected:
  GUI();

  Rectangle mBoundingBox;
  Rectangle mMouseoverBox;

  vec2 mOffset;
  bool mIsVisible;
  bool mIsClickable;
  bool mIsAnimating;
  bool mIsMinimized;
  bool mHasChanged;
  bool mIsMouseOver;

  std::shared_ptr<Program> mGUIProgram;
  std::function<void(const Input::Event& event)> mInputHandler;
};
