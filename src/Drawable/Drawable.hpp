#pragma once

class Asset;

namespace Input {
  class Event;
}

class Drawable {
public:
  virtual ~Drawable();

  // Update the element, phusics etc
  virtual void update(float deltaTime) = 0;

  virtual void input(const Input::Event& event) = 0;

  static Asset* mAsset;

protected:
  Drawable();
};
