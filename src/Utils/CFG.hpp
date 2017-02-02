#pragma once

#include <functional>
#include <map>
#include <string>
#include <vector>

#include <mmm.hpp>

//! Actions bindings - Keybindings
struct ActB {
  ActB(int k1 = -1, int k2 = -1);

  int key1;
  int key2;
};

class CFG {
public:
  typedef std::string        Prop;
  typedef std::string        Param;
  typedef std::vector<Param> Params;

  struct Graphics {
    int       aliasing      = 0;
    float     aniso         = 0;
    float     aspect        = 1.33333333;
    int       monitor       = 0;
    mmm::vec2 res           = { 1024, 768 };
    int       shadowSamples = 4;
    int       shadowRes     = 1024;
    bool      vsync         = false;
    int       viewDistance  = 20;
    int       winMode       = 0;
  } graphics;

  struct General {
    bool debug = false;
  } general;

  struct Camera {
    float rotSpeed  = 360;
    bool  rotInvH   = false;
    bool  rotInvV   = false;
    float zoomSpeed = 0.5;
    bool  zoomInv   = false;
  } camera;

  struct Console {
    bool enabled = false;
  } console;

  struct Bindings {
    ActB moveUp      = ActB(87);
    ActB moveDown    = ActB(83);
    ActB moveLeft    = ActB(65);
    ActB moveRight   = ActB(68);
    ActB pauseMenu   = ActB(256);
    ActB rotate      = ActB(2);
    ActB showConsole = ActB(96);
    ActB screenshot  = ActB();
  } bindings;

  struct Audio {
    float SFXVolume    = 0.5;
    float musicVolume  = 0.5;
    float masterVolume = 0.5;
  } audio;


  CFG();

  //! set a property using string properties \n
  //! e.g. cfg.setProp("Graphics.resolution", {"1920", "1080"});
  void setProp(const Prop& p, const Params& params);

  //! gets a string property using string properties
  //! e.g. cfg.getProp("Graphics.resolution");
  std::string getProp(const Prop& p);

  //! merges the default cfg with settings from a config file
  void assimilate(const std::string& filepath);

  //! merges the default cfg with settings from command line
  void assimilate(int argc, char* argv[]);

  //! Writes current settings to file
  bool writetoFile(std::string filename);

  //! useful for printing to file
  friend std::ostream& operator<<(std::ostream& os, const CFG& cfg);


private:
// hidden implementation details
#include "CFG.tpp"

  // may have undefined behaviour if a key name is longer than 22 characters
  std::map<Prop, Wrapper>
    map{ { "Graphics.anisotropicfiltering",
           { graphics.aniso,
             { { "off", 1 },
               { "2x", 2 },
               { "4x", 4 },
               { "8x", 8 },
               { "16x", 16 } } } },
         { "Graphics.antialiasing",
           { graphics.aliasing,
             { { "off", 1 },
               { "2x", 2 },
               { "4x", 4 },
               { "8x", 8 },
               { "16x", 16 } } } },
         { "Graphics.monitor", graphics.monitor },
         { "Graphics.resolution", graphics.res },
         { "Graphics.resolution.x", { graphics.res.x } },
         { "Graphics.resolution.y", { graphics.res.y } },
         { "Graphics.shadow_samples",
           { graphics.shadowSamples,
             { { "off", 1 },
               { "2x", 2 },
               { "4x", 4 },
               { "8x", 8 },
               { "16x", 16 } } } },
         { "Graphics.shadow_resolution",
           { graphics.shadowRes,
             { { "low", 512 },
               { "medium", 1024 },
               { "high", 2048 },
               { "ultra", 4096 } } } },
         { "Graphics.view_distance",
           { graphics.viewDistance,
             { { "low", 20 },
               { "medium", 40 },
               { "high", 60 },
               { "ultra", 80 } } } },
         { "Graphics.vsync", graphics.vsync },
         { "Graphics.windowmode",
           { graphics.winMode,
             { { "window", 0 }, { "borderless", 1 }, { "fullscreen", 2 } } } },

         { "General.debug", general.debug },

         { "Camera.rotation_speed", camera.rotSpeed },
         { "Camera.rotation_inverse_horizontal", camera.rotInvH },
         { "Camera.rotation_inverse_vertical", camera.rotInvV },
         { "Camera.zoom_speed", camera.zoomSpeed },
         { "Camera.zoom_inverse", camera.zoomInv },

         { "Bindings.move_up", bindings.moveUp },
         { "Bindings.move_down", bindings.moveDown },
         { "Bindings.move_left", bindings.moveLeft },
         { "Bindings.move_right", bindings.moveRight },
         { "Bindings.pause_menu", bindings.pauseMenu },
         { "Bindings.rotate", bindings.rotate },

         { "Console.enabled", console.enabled },

         { "Bindings.screenshot", bindings.screenshot },
         { "Bindings.show_console", bindings.showConsole },

         { "Audio.sfx_volume", audio.SFXVolume },
         { "Audio.music_volume", audio.musicVolume },
         { "Audio.master_volume", audio.masterVolume } };

  // duplicate properties that will not be printed to file
  std::vector<Prop> duplicates{ "Graphics.resolution.x",
                                "Graphics.resolution.y" };

  void special_cases() {

    map.at("Graphics.resolution").add_special_case([this]() {
      graphics.aspect = graphics.res.x / graphics.res.y;
    });

    map.at("Graphics.resolution.x").add_special_case([this]() {
      graphics.aspect = graphics.res.x / graphics.res.y;
    });

    map.at("Graphics.resolution.y").add_special_case([this]() {
      graphics.aspect = graphics.res.x / graphics.res.y;
    });
  }
};
