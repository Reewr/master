#pragma once

class btRigidBody;
class Asset;

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

  virtual bool hasPhysics() = 0;

  virtual btRigidBody* getRigidBody() = 0;

  virtual void updateFromPhysics() = 0;

  static Asset* mAsset;

protected:

  Drawable();
};
