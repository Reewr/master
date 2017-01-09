#ifndef STATE_STATE_HPP
#define STATE_STATE_HPP

struct Asset;

//! Fully Virtual class utilized by StateGame and StateMenu
class State {
public:

  enum {
    QUITALL = -2,
    QUIT = -1,
    INIT,
    MAINMENU,
    GAME,
    MASTER_THESIS,
    REFRESH,
    WINREFRESH,
    NOCHANGE
  };

  virtual ~State();
  virtual void update(float deltaTime) = 0;
  virtual int keyboardCB(int key, int scan, int action, int mods) = 0;
  virtual void mouseMovementCB(double x, double y) = 0;
  virtual int mouseButtonCB(int button, int action, int mods) = 0;
  virtual void mouseScrollCB(double offsetx, double offsety) = 0;

protected:
  State();

  virtual void draw3D() = 0;
  virtual void drawGUI() = 0;

  float mDeltaTime;
};

#endif