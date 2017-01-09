#ifndef ANIMATION_STATE_HPP
#define ANIMATION_STATE_HPP

#include <memory>
#include <functional>
#include <Animation/Data.hpp>

namespace Animation {
  typedef std::function<float()> Var;

  template <size_t n>
  struct State {

    constexpr State (float start, float end);
    virtual  ~State ();

    virtual constexpr State<n>* copy    () = 0;
    virtual constexpr float     length  ();
    virtual           State<n>* advance (float dt);
    virtual constexpr Frame<n>  current () = 0;
    virtual constexpr Frame<n>  at      (float) = 0;

                      Data<n>*  compile (int frames);

    float start;
    float end;
  };

  #include "State.tpp"
}

#endif