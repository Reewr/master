#pragma once


namespace Input {
  class Event;
}

class Drawable {
public:
  virtual ~Drawable();

  // Update the element, phusics etc
  virtual void update(float deltaTime) = 0;

  // Draw it. Keep it seperate from update
  virtual void draw(float deltaTime) = 0;

  virtual void input(const Input::Event& event) = 0;

protected:

  Drawable();
};
