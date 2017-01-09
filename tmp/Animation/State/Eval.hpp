#ifndef ANIMATION_EVAL_HPP
#define ANIMATION_EVAL_HPP 

#include <Animation/State/State.hpp>

namespace Animation {

  template <size_t n>
  struct Anim;

  template <size_t n>
  struct Eval : State<n> {

    constexpr Eval (Var c, State<n>* state);
    constexpr Eval (Var c, Anim<n>&& anim);
    constexpr Eval (Var c, const Anim<n>& anim);

    constexpr State<n>* copy      ();
              State<n>* advance   (float);
    constexpr Frame<n>  current   ();
    constexpr Frame<n>  at        (float);

    private:
    constexpr Eval (Var c, State<n>* state, float start, float end);

    const Var                       var;
          std::unique_ptr<State<n>> state;
  };

  #include "Eval.tpp"
}

#endif