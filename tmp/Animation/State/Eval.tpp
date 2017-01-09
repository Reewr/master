template <size_t n>
constexpr Eval<n>::Eval (Var v, State<n>* state) :
  State<n>(0, state->length()), var(v), state(state) {}

template <size_t n>
constexpr Eval<n>::Eval (Var v, State<n>* state, float start, float end) :
  State<n>(start, end), var(v), state(state) {}


template <size_t n>
constexpr State<n>* Eval<n>::copy () {
  return new Eval<n>(var, state->copy(), this->start, this->end);
}
template <size_t n>
State<n>* Eval<n>::advance (float) {
  return this;
}


template <size_t n>
constexpr Frame<n> Eval<n>::current () {
  return state->at(var());
}

template <size_t n>
constexpr Frame<n> Eval<n>::at (float) {
  return state->at(var());
}
