#ifndef ANIMATION_TRANSITION_HPP
#define ANIMATION_TRANSITION_HPP

#include <memory>
#include <Animation/State/State.hpp>

namespace Animation {

  template <size_t n>
  struct Anim;

  template <size_t n>
  struct Transition : State<n> {

    constexpr Transition (Anim<n>&& anim1, Anim<n>&& anim2);
    constexpr Transition (const Anim<n>& anim1, const Anim<n>& anim2);
    constexpr Transition (State<n>*const state1, State<n>*const state2);

    constexpr State<n>* copy    ();
    constexpr Frame<n>  current ();
    constexpr Frame<n>  at      (float t);

    private:
    constexpr Transition (State<n>*const state1, State<n>*const state2, float start, float end);
    constexpr Frame<n> _at (float t);

    State<n> * const state1;
    State<n> * const state2;
  };

  #include "Transition.tpp"
}

#endif