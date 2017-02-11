#pragma once

class Asset;

namespace Input {
  class Event;
}

/**
 * @brief
 *   This class represents all drawable objects. It is the base object that
 *   every other object inherits from.
 *
 *   Kind of ironic that it does not have a draw function though. This is
 *   because Drawable3D and Drawable2D (GUI) extends this class.
 */
class Drawable {
public:
  virtual ~Drawable();

  // Update the element, physics etc
  virtual void update(float deltaTime) = 0;

  // Update based on input
  virtual void input(const Input::Event& event) = 0;

  static Asset* mAsset;

protected:
  Drawable();
};
