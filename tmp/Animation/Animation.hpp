#ifndef ANIMATION_HPP
#define ANIMATION_HPP 

#include <memory>

#include <Animation/State/State.hpp>
#include <Animation/State/Pose.hpp>
#include <Animation/State/Basic.hpp>
#include <Animation/State/Reverse.hpp>
#include <Animation/State/Sequence.hpp>
#include <Animation/State/Transition.hpp>
#include <Animation/State/Blend.hpp>
#include <Animation/State/Eval.hpp>
#include <Animation/State/Loop.hpp>

namespace Animation {

  template <size_t n>
  struct Anim {

    std::unique_ptr<State<n>> state;

    constexpr Anim ();
    constexpr Anim (Frame<n>&& frame);
    constexpr Anim (const Frame<n>& frame);
    constexpr Anim (Data<n> const * const data);
    constexpr Anim (State<n>* state);
    constexpr Anim (Anim<n>&& b);
    constexpr Anim (const Anim<n>& b);

    // advance operator
    constexpr Frame<n> operator () (float dt);

    // at operator
    constexpr Frame<n> operator [] (float t);
    constexpr Anim<n>  operator [] (Var v);

    // sequence operator
    Anim<n>& operator >= (Anim<n>&& b);
    Anim<n>& operator >= (const Anim<n>& b);

    // blending operators
    Anim<n>& operator += (float s);
    Anim<n>& operator += (Var s);
    Anim<n>& operator += (Anim<n>&& b);
    Anim<n>& operator += (const Anim<n>& b);
    Anim<n>& operator -= (float s);
    Anim<n>& operator -= (Var s);
    Anim<n>& operator -= (Anim<n>&& b);
    Anim<n>& operator -= (const Anim<n>& b);
    Anim<n>& operator *= (float s);
    Anim<n>& operator *= (Var s);
    Anim<n>& operator *= (Anim<n>&& b);
    Anim<n>& operator *= (const Anim<n>& b);
    Anim<n>& operator /= (float s);
    Anim<n>& operator /= (Var s);
    Anim<n>& operator /= (Anim<n>&& b);
    Anim<n>& operator /= (const Anim<n>& b);

    Anim<n>& operator =  (Anim<n>&& b);
    Anim<n>& operator =  (const Anim<n>& b);

    Data<n>* compile     (int frames);

    #include "Animation_inline.tpp"
  };

  template <size_t n>
  constexpr Anim<n> transition (Anim<n>&& a, Anim<n>&& b);
  template <size_t n>
  constexpr Anim<n> transition (Anim<n>&& a, const Anim<n>& b);
  template <size_t n>
  constexpr Anim<n> transition (const Anim<n>& a, Anim<n>&& b);
  template <size_t n>
  constexpr Anim<n> transition (const Anim<n>& a, const Anim<n>& b);

  template <size_t n>
  constexpr Anim<n> loop (Anim<n>&& a);
  template <size_t n>
  constexpr Anim<n> loop (const Anim<n>& a);

  // transition function
  // overlay function

  template <size_t n>
  constexpr Anim<n> operator > (Anim<n>&& a, Anim<n>&& b);
  template <size_t n>
  constexpr Anim<n> operator > (Anim<n>&& a, const Anim<n>& b);
  template <size_t n>
  constexpr Anim<n> operator > (const Anim<n>& a, Anim<n>&& b);
  template <size_t n>
  constexpr Anim<n> operator > (const Anim<n>& a, const Anim<n>& b);

  template <size_t n>
  constexpr Anim<n> operator + (float s, Anim<n>&& b);
  template <size_t n>
  constexpr Anim<n> operator + (float s, const Anim<n>& b);
  template <size_t n>
  constexpr Anim<n> operator + (Var s, Anim<n>&& b);
  template <size_t n>
  constexpr Anim<n> operator + (Var s, const Anim<n>& b);
  template <size_t n>
  constexpr Anim<n> operator + (Anim<n>&& a, float s);
  template <size_t n>
  constexpr Anim<n> operator + (const Anim<n>& a, float s);
  template <size_t n>
  constexpr Anim<n> operator + (Anim<n>&& a, Var s);
  template <size_t n>
  constexpr Anim<n> operator + (const Anim<n>& a, Var s);
  template <size_t n>
  constexpr Anim<n> operator + (Anim<n>&& a, Anim<n>&& b);
  template <size_t n>
  constexpr Anim<n> operator + (Anim<n>&& a, const Anim<n>& b);
  template <size_t n>
  constexpr Anim<n> operator + (const Anim<n>& a, Anim<n>&& b);
  template <size_t n>
  constexpr Anim<n> operator + (const Anim<n>& a, const Anim<n>& b);

  template <size_t n>
  constexpr Anim<n> operator - (Anim<n>&& a);
  template <size_t n>
  constexpr Anim<n> operator - (const Anim<n>& a);

  template <size_t n>
  constexpr Anim<n> operator - (float s, Anim<n>&& b);
  template <size_t n>
  constexpr Anim<n> operator - (float s, const Anim<n>& b);
  template <size_t n>
  constexpr Anim<n> operator - (Var s, Anim<n>&& b);
  template <size_t n>
  constexpr Anim<n> operator - (Var s, const Anim<n>& b);
  template <size_t n>
  constexpr Anim<n> operator - (Anim<n>&& a, float s);
  template <size_t n>
  constexpr Anim<n> operator - (const Anim<n>& a, float s);
  template <size_t n>
  constexpr Anim<n> operator - (Anim<n>&& a, Var s);
  template <size_t n>
  constexpr Anim<n> operator - (const Anim<n>& a, Var s);
  template <size_t n>
  constexpr Anim<n> operator - (Anim<n>&& a, Anim<n>&& b);
  template <size_t n>
  constexpr Anim<n> operator - (Anim<n>&& a, const Anim<n>& b);
  template <size_t n>
  constexpr Anim<n> operator - (const Anim<n>& a, Anim<n>&& b);
  template <size_t n>
  constexpr Anim<n> operator - (const Anim<n>& a, const Anim<n>& b);

  template <size_t n>
  constexpr Anim<n> operator * (float s, Anim<n>&& b);
  template <size_t n>
  constexpr Anim<n> operator * (float s, const Anim<n>& b);
  template <size_t n>
  constexpr Anim<n> operator * (Var s, Anim<n>&& b);
  template <size_t n>
  constexpr Anim<n> operator * (Var s, const Anim<n>& b);
  template <size_t n>
  constexpr Anim<n> operator * (Anim<n>&& a, float s);
  template <size_t n>
  constexpr Anim<n> operator * (const Anim<n>& a, float s);
  template <size_t n>
  constexpr Anim<n> operator * (Anim<n>&& a, Var s);
  template <size_t n>
  constexpr Anim<n> operator * (const Anim<n>& a, Var s);
  template <size_t n>
  constexpr Anim<n> operator * (Anim<n>&& a, Anim<n>&& b);
  template <size_t n>
  constexpr Anim<n> operator * (Anim<n>&& a, const Anim<n>& b);
  template <size_t n>
  constexpr Anim<n> operator * (const Anim<n>& a, Anim<n>&& b);
  template <size_t n>
  constexpr Anim<n> operator * (const Anim<n>& a, const Anim<n>& b);

  template <size_t n>
  constexpr Anim<n> operator / (float s, Anim<n>&& b);
  template <size_t n>
  constexpr Anim<n> operator / (float s, const Anim<n>& b);
  template <size_t n>
  constexpr Anim<n> operator / (Var s, Anim<n>&& b);
  template <size_t n>
  constexpr Anim<n> operator / (Var s, const Anim<n>& b);
  template <size_t n>
  constexpr Anim<n> operator / (Anim<n>&& a, float s);
  template <size_t n>
  constexpr Anim<n> operator / (const Anim<n>& a, float s);
  template <size_t n>
  constexpr Anim<n> operator / (Anim<n>&& a, Var s);
  template <size_t n>
  constexpr Anim<n> operator / (const Anim<n>& a, Var s);
  template <size_t n>
  constexpr Anim<n> operator / (Anim<n>&& a, Anim<n>&& b);
  template <size_t n>
  constexpr Anim<n> operator / (Anim<n>&& a, const Anim<n>& b);
  template <size_t n>
  constexpr Anim<n> operator / (const Anim<n>& a, Anim<n>&& b);
  template <size_t n>
  constexpr Anim<n> operator / (const Anim<n>& a, const Anim<n>& b);

  #include "Animation.tpp"
}

#endif