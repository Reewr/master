#ifndef ANIMATION_SEQUENCE_HPP
#define ANIMATION_SEQUENCE_HPP 

#include <Animation/State/State.hpp>

namespace Animation {

  template <size_t n>
  struct Anim;

  template <size_t n>
  struct Sequence : State<n> {

    constexpr Sequence (Anim<n>&& anim1, Anim<n>&& anim2);
    constexpr Sequence (const Anim<n>& anim1, const Anim<n>& anim2);
    constexpr Sequence (State<n>* state1, State<n>* state2);

    constexpr State<n>* copy       ();
    constexpr Frame<n>  current    ();
    constexpr Frame<n>  at         (float t);

    private:
    constexpr Sequence (State<n>* state1, State<n>* state2, float start, float end);
    std::unique_ptr<State<n>> state1;
    std::unique_ptr<State<n>> state2;
  };

  #include "Sequence.tpp"
}

#endif