#ifndef ANIMATION_REVERSE_HPP
#define ANIMATION_REVERSE_HPP

#include <memory>
#include <Animation/State/State.hpp>

namespace Animation {

  template <size_t n>
  struct Anim;

  template <size_t n>
  struct Reverse : State<n> {

    constexpr Reverse (Anim<n>&& anim);
    constexpr Reverse (const Anim<n>& anim);
    constexpr Reverse (State<n>* state);

    constexpr State<n>* copy    ();
    constexpr Frame<n>  current ();
    constexpr Frame<n>  at      (float t);



    private:
    constexpr Reverse (State<n>* state, float start, float end);
    std::unique_ptr<State<n>> state;
  };

  #include "Reverse.tpp"
}

#endif