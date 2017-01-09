#ifndef ANIMATION_LOOP_HPP
#define ANIMATION_LOOP_HPP 

#include <Animation/State/State.hpp>

namespace Animation {

  template <size_t n>
  struct Anim;

  template <size_t n>
  struct Loop : State<n> {

    constexpr Loop (Anim<n>&& anim);
    constexpr Loop (const Anim<n>& anim);
    constexpr Loop (State<n>* state);

    constexpr State<n>* copy      ();
              State<n>* advance   (float dt);
    constexpr Frame<n>  current   ();
    constexpr Frame<n>  at        (float t);

    private:
    constexpr Loop (State<n>* state, float start, float acc, float end);

    float                     acc;
    std::unique_ptr<State<n>> state;
  };

  #include "Loop.tpp"
}

#endif