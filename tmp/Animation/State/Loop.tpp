template <size_t n>
constexpr Loop<n>::Loop (State<n>* state) :
  State<n>(0, state->length()), acc(0), state(state) {}

template <size_t n>
constexpr Loop<n>::Loop (State<n>* state, float start, float acc, float end) :
  State<n>(start, end), acc(acc), state(state) {}

template <size_t n>
constexpr State<n>* Loop<n>::copy () {
  return new Loop<n>(state->copy(), this->start, acc, this->end);
}

template <size_t n>
State<n>* Loop<n>::advance (float dt) {
  acc = mod (acc+dt, this->length()) + this->start;
  return this;
}

template <size_t n>
constexpr Frame<n> Loop<n>::current () {
  return state->at(acc);
}

template <size_t n>
constexpr Frame<n> Loop<n>::at (float t) {
  return state->at(mod(t, this->length()) + this->start);
}
