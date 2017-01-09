#ifndef DATA_HPP
#define DATA_HPP 

#include <iostream>
#include <iomanip>
#include <vector>

#include <Animation/Frame.hpp>
#include <Import/Animation.hpp>
#include <Utils/Utils.hpp>

namespace Animation {

  template <size_t n>
  struct KeyFrame {
    const float    t;
    const Frame<n> f;

    constexpr KeyFrame ();
    constexpr KeyFrame (float t, Frame<n>&& f);
    constexpr KeyFrame (float t, const Frame<n>& f);
    template <typename...floats>
    constexpr KeyFrame (float t, floats...fs);
  };

  template <size_t n>
  struct Data {

    const std::vector<KeyFrame<n>> fs;
    
    template <typename...KFS>
    constexpr Data (KeyFrame<n>&& f, KFS...fs);
    template <typename...KFS>
    constexpr Data (const KeyFrame<n>& f, KFS...fs);

    constexpr Data (std::vector<KeyFrame<n>>&& fs);
    constexpr Data (const std::vector<KeyFrame<n>>& fs);

    Data (const char* filePath);

    constexpr float length ();

    constexpr Frame<n> interpolate (float acc);
  
    private:
    constexpr Frame<n> interpolate_recursive (float acc, size_t i);
    static std::vector<KeyFrame<n>> import (const char* filePath);
  };

  template <size_t n>
  void print (const KeyFrame<n>& f);

  template <size_t n>
  void print (const KeyFrame<n>& f);

  #include "Data.tpp"
}

#endif