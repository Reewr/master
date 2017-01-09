#ifndef ANIMATION_BASIC_HPP
#define ANIMATION_BASIC_HPP 

#include <Animation/State/State.hpp>

namespace Animation {

  template <size_t n>
  struct Basic : State<n> {

    constexpr Basic (Data<n> const * const data);

    constexpr State<n>* copy    ();
    constexpr Frame<n>  current ();
    constexpr Frame<n>  at      (float t);

    private:
    constexpr Basic (Data<n> const * const data, float start, float end);
    Data<n> const * const data;
  };

  #include "Basic.tpp"
}

#endif