#ifndef ANIMATION_BLEND2_HPP
#define ANIMATION_BLEND2_HPP 

#include <Animation/State/State.hpp>

namespace Animation {

  template <size_t n>
  struct Anim;

  template <size_t n, typename...AS>
  struct Blend : State<n> {};


  // Unary blend
  template <size_t n>
  struct Blend <n,State<n>> : State<n> {
    typedef Frame<n> (*Func)(const Frame<n>&);

    constexpr Blend (Func blend, Anim<n>&& anim);
    constexpr Blend (Func blend, const Anim<n>& anim);
    constexpr Blend (Func blend, State<n>* state);

    constexpr State<n>* copy       ();
    constexpr Frame<n>  current    ();
    constexpr Frame<n>  at         (float t);

    private:
    constexpr Blend (Func blend, State<n>* state, float start, float end);

    const Func blend;
    std::unique_ptr<State<n>> state;
  };


  // Scalar blend
  template <size_t n>
  struct Blend <n,float,State<n>> : State<n> {
    typedef Frame<n> (*Func)(float, const Frame<n>&);

    constexpr Blend (Func blend, float s, Anim<n>&& anim);
    constexpr Blend (Func blend, float s, const Anim<n>& anim);
    constexpr Blend (Func blend, float s, State<n>* state);

    constexpr State<n>* copy       ();
    constexpr Frame<n>  current    ();
    constexpr Frame<n>  at         (float t);

    private:
    constexpr Blend (Func blend, float s, State<n>* state, float start, float end);

    const Func                      blend;
    const float                     scalar;
          std::unique_ptr<State<n>> state;
  };

  template <size_t n>
  struct Blend <n,Var,State<n>> : State<n> {
    typedef Frame<n> (*Func)(float, const Frame<n>&);

    constexpr Blend (Func blend, Var s, Anim<n>&& anim);
    constexpr Blend (Func blend, Var s, const Anim<n>& anim);
    constexpr Blend (Func blend, Var s, State<n>* state);

    constexpr State<n>* copy       ();
    constexpr Frame<n>  current    ();
    constexpr Frame<n>  at         (float t);

    private:
    constexpr Blend (Func blend, Var s, State<n>* state, float start, float end);

    const Func                      blend;
    const Var                       scalar;
          std::unique_ptr<State<n>> state;
  };

  template <size_t n>
  struct Blend <n,State<n>,float> : State<n> {
    typedef Frame<n> (*Func)(const Frame<n>&, float);

    constexpr Blend (Func blend, Anim<n>&& anim, float s);
    constexpr Blend (Func blend, const Anim<n>& anim, float s);
    constexpr Blend (Func blend, State<n>* state, float s);

    constexpr State<n>* copy       ();
    constexpr Frame<n>  current    ();
    constexpr Frame<n>  at         (float t);

    private:
    constexpr Blend (Func blend, State<n>* state, float s, float start, float end);

    const Func                      blend;
          std::unique_ptr<State<n>> state;
    const float                     scalar;
  };

  template <size_t n>
  struct Blend <n,State<n>,Var> : State<n> {
    typedef Frame<n> (*Func)(const Frame<n>&, float);

    constexpr Blend (Func blend, Anim<n>&& anim, Var s);
    constexpr Blend (Func blend, const Anim<n>& anim, Var s);
    constexpr Blend (Func blend, State<n>* state, Var s);

    constexpr State<n>* copy       ();
    constexpr Frame<n>  current    ();
    constexpr Frame<n>  at         (float t);

    private:
    constexpr Blend (Func blend, State<n>* state, Var s, float start, float end);

    const Func                      blend;
          std::unique_ptr<State<n>> state;
    const Var                       scalar;
  };


  // Binary blend
  template <size_t n>
  struct Blend <n,State<n>,State<n>> : State<n> {
    typedef Frame<n> (*Func)(const Frame<n>&, const Frame<n>&);

    constexpr Blend (Func blend, Anim<n>&& anim1, Anim<n>&& anim2);
    constexpr Blend (Func blend, const Anim<n>& anim1, const Anim<n>& anim2);
    constexpr Blend (Func blend, State<n>* state1, State<n>* state2);

    constexpr State<n>* copy       ();
    constexpr Frame<n>  current    ();
    constexpr Frame<n>  at         (float t);

    private:
    constexpr Blend (Func blend, State<n>* state1, State<n>* state2, float start, float end);

    const Func blend;
    std::unique_ptr<State<n>> state1;
    std::unique_ptr<State<n>> state2;
  };


  namespace Blends {

    template <typename A, typename B, typename C>
    constexpr C add (A x, B y);

    template <typename A, typename B>
    constexpr B neg (A x);
    
    template <typename A, typename B, typename C>
    constexpr C sub (A x, B y);

    template <typename A, typename B, typename C>
    constexpr C mult (A x, B y);

    template <typename A, typename B, typename C>
    constexpr C div (A x, B y);
  }

  #include "Blend.tpp"
}

#endif