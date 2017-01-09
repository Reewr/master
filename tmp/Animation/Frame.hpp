#ifndef ANIMATION_FRAME_HPP
#define ANIMATION_FRAME_HPP 

#include <iostream>
#include <vector>
#include <Math/Math.hpp>

namespace Animation {

  // Frame is a specialized pairwise combinator.
  template <size_t n>
  struct Frame {

    static_assert(n!=0, "Animation::Frame must have at least 1 element");

    float      x;
    Frame<n-1> xs;

    constexpr Frame ();
    constexpr Frame (float x);
    constexpr Frame (float x, Frame<n-1>&& xs);
    constexpr Frame (float x, const Frame<n-1>& xs);
    template <typename...floats>
    constexpr Frame (float x, float y, floats...xs);

    Frame (const std::vector<float>& g);

    constexpr float operator [] (size_t i);
    float& operator [] (size_t i);

    Frame<n>& operator += (float s);
    Frame<n>& operator += (const Frame<n>& g);
    Frame<n>& operator -= (float s);
    Frame<n>& operator -= (const Frame<n>& g);
    Frame<n>& operator *= (float s);
    Frame<n>& operator *= (const Frame<n>& g);
    Frame<n>& operator /= (float s);
    Frame<n>& operator /= (const Frame<n>& g);
  };

  template <>
  struct Frame <1> {

    float x;

    constexpr Frame ();
    constexpr Frame (float x);
    
    Frame (const std::vector<float>& g);

    constexpr float operator [] (size_t i);
    float& operator [] (size_t i);

    Frame<1>& operator += (float s);
    Frame<1>& operator += (const Frame<1>& g);
    Frame<1>& operator -= (float s);
    Frame<1>& operator -= (const Frame<1>& g);
    Frame<1>& operator *= (float s);
    Frame<1>& operator *= (const Frame<1>& g);
    Frame<1>& operator /= (float s);
    Frame<1>& operator /= (const Frame<1>& g);
  };

  template <size_t n>
  constexpr Frame<n> operator + (float s, const Frame<n>& g);
  template <size_t n>
  constexpr Frame<n> operator + (const Frame<n>& f, float s);
  template <size_t n>
  constexpr Frame<n> operator + (const Frame<n>& f, const Frame<n>& g);

  template <size_t n>
  constexpr Frame<n> operator - (const Frame<n>& f);
  template <size_t n>
  constexpr Frame<n> operator - (float s, const Frame<n>& g);
  template <size_t n>
  constexpr Frame<n> operator - (const Frame<n>& f, float s);
  template <size_t n>
  constexpr Frame<n> operator - (const Frame<n>& f, const Frame<n>& g);

  template <size_t n>
  constexpr Frame<n> operator * (float s, const Frame<n>& g);
  template <size_t n>
  constexpr Frame<n> operator * (const Frame<n>& f, float s);
  template <size_t n>
  constexpr Frame<n> operator * (const Frame<n>& f, const Frame<n>& g);

  template <size_t n>
  constexpr Frame<n> operator / (float s, const Frame<n>& g);
  template <size_t n>
  constexpr Frame<n> operator / (const Frame<n>& f, float s);
  template <size_t n>
  constexpr Frame<n> operator / (const Frame<n>& f, const Frame<n>& g);

  template <size_t n>
  constexpr bool operator == (const Frame<n>& f, const Frame<n>& g);
  template <size_t n>
  constexpr bool operator != (const Frame<n>& f, const Frame<n>& g);

  template <size_t n>
  std::ostream& operator << (std::ostream& os, const Frame<n>& f);

  template <size_t n>
  void print (const Frame<n>& f);

  #include "Frame.tpp"
}

#endif