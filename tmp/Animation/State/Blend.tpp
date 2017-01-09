template <typename A, typename B, typename C>
constexpr C Blends::add (A x, B y) {return x + y;}

template <typename A, typename B>
constexpr B Blends::neg (A x) {return -x;}

template <typename A, typename B, typename C>
constexpr C Blends::sub (A x, B y) {return x - y;}

template <typename A, typename B, typename C>
constexpr C Blends::mult (A x, B y) {return x * y;}

template <typename A, typename B, typename C>
constexpr C Blends::div (A x, B y) {return x / y;}


template <size_t n>
constexpr Blend<n,State<n>>::Blend (
  Func blend, State<n>* state) :
    State<n>(0, state->length()), blend(blend), state(state) {}

template <size_t n>
constexpr Blend<n,State<n>>::Blend (
  Func blend, State<n>* state, float start, float end) :
    State<n>(start, end), blend(blend), state(state) {}

template <size_t n>
constexpr Blend<n,float,State<n>>::Blend (
  Func blend, float s, State<n>* state) :
    State<n>(0, state->length()), blend(blend), scalar(s), state(state) {}

template <size_t n>
constexpr Blend<n,float,State<n>>::Blend (
  Func blend, float s, State<n>* state, float start, float end) :
    State<n>(start, end), blend(blend), scalar(s), state(state) {}

template <size_t n>
constexpr Blend<n,Var,State<n>>::Blend (
  Func blend, Var s, State<n>* state) :
    State<n>(0, state->length()), blend(blend), scalar(s), state(state) {}

template <size_t n>
constexpr Blend<n,Var,State<n>>::Blend (
  Func blend, Var s, State<n>* state, float start, float end) :
    State<n>(start, end), blend(blend), scalar(s), state(state) {}

template <size_t n>
constexpr Blend<n,State<n>,float>::Blend (
  Func blend, State<n>* state, float s) :
    State<n>(0, state->length()), blend(blend), state(state), scalar(s) {}

template <size_t n>
constexpr Blend<n,State<n>,float>::Blend (
  Func blend, State<n>* state, float s, float start, float end) :
    State<n>(start, end), blend(blend), state(state), scalar(s) {}

template <size_t n>
constexpr Blend<n,State<n>,Var>::Blend (
  Func blend, State<n>* state, Var s) :
    State<n>(0, state->length()), blend(blend), state(state), scalar(s) {}

template <size_t n>
constexpr Blend<n,State<n>,Var>::Blend (
  Func blend, State<n>* state, Var s, float start, float end) :
    State<n>(start, end), blend(blend), state(state), scalar(s) {}

template <size_t n>
constexpr Blend<n,State<n>,State<n>>::Blend (
  Func blend, State<n>* state1, State<n>* state2) :
    State<n>(0, fmax(state1->length(), state2->length())),
    blend(blend), state1(state1), state2(state2) {}

template <size_t n>
constexpr Blend<n,State<n>,State<n>>::Blend (
  Func blend, State<n>* state1, State<n>* state2, float start, float end) :
    State<n>(start, end), blend(blend), state1(state1), state2(state2) {}


template <size_t n>
constexpr State<n>* Blend<n,State<n>>::copy () {
  return new Blend<n,State<n>>(
    blend, state->copy(), this->start, this->end);
}
template <size_t n>
constexpr State<n>* Blend<n,float,State<n>>::copy () {
  return new Blend<n,float,State<n>>(
    blend, scalar, state->copy(), this->start, this->end);
}
template <size_t n>
constexpr State<n>* Blend<n,Var,State<n>>::copy () {
  return new Blend<n,Var,State<n>>(
    blend, scalar, state->copy(), this->start, this->end);
}
template <size_t n>
constexpr State<n>* Blend<n,State<n>,float>::copy () {
  return new Blend<n,State<n>,float>(
    blend, state->copy(), scalar, this->start, this->end);
}
template <size_t n>
constexpr State<n>* Blend<n,State<n>,Var>::copy () {
  return new Blend<n,State<n>,Var>(
    blend, state->copy(), scalar, this->start, this->end);
}
template <size_t n>
constexpr State<n>* Blend<n,State<n>,State<n>>::copy () {
  return new Blend<n,State<n>,State<n>>(
    blend, state1->copy(), state2->copy(), this->start, this->end);
}


template <size_t n>
constexpr Frame<n> Blend<n,State<n>>::current () {
  return blend(state->at(this->start));
}
template <size_t n>
constexpr Frame<n> Blend<n,float,State<n>>::current () {
  return blend(scalar, state->at(this->start));
}
template <size_t n>
constexpr Frame<n> Blend<n,Var,State<n>>::current () {
  return blend(scalar(), state->at(this->start));
}
template <size_t n>
constexpr Frame<n> Blend<n,State<n>,float>::current () {
  return blend(state->at(this->start), scalar);
}
template <size_t n>
constexpr Frame<n> Blend<n,State<n>,Var>::current () {
  return blend(state->at(this->start), scalar());
}
template <size_t n>
constexpr Frame<n> Blend<n,State<n>,State<n>>::current () {
  return blend(state1->at(this->start), state2->at(this->start));
}


template <size_t n>
constexpr Frame<n> Blend<n,State<n>>::at (float t) {
  return blend(state->at(clamp(t, this->start, this->end)));
}
template <size_t n>
constexpr Frame<n> Blend<n,float,State<n>>::at (float t) {
  return blend(scalar, state->at(clamp(t, this->start, this->end)));
}
template <size_t n>
constexpr Frame<n> Blend<n,Var,State<n>>::at (float t) {
  return blend(scalar(), state->at(clamp(t, this->start, this->end)));
}
template <size_t n>
constexpr Frame<n> Blend<n,State<n>,float>::at (float t) {
  return blend(state->at(clamp(t, this->start, this->end)), scalar);
}
template <size_t n>
constexpr Frame<n> Blend<n,State<n>,Var>::at (float t) {
  return blend(state->at(clamp(t, this->start, this->end)), scalar());
}
template <size_t n>
constexpr Frame<n> Blend<n,State<n>,State<n>>::at (float t) {
  return blend(
    state1->at(clamp(t, this->start, this->end)),
    state2->at(clamp(t, this->start, this->end))
  );
}
