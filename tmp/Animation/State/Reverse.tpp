template <size_t n>
constexpr Reverse<n>::Reverse (State<n>* state) :
  State<n>(0, state->length()), state(state) {}

template <size_t n>
constexpr Reverse<n>::Reverse (State<n>* state, float start, float end) :
  State<n>(start, end), state(state) {}

template <size_t n>
constexpr State<n>* Reverse<n>::copy () {
  return new Reverse<n>(state->copy(), this->start, this->end);
}

template <size_t n>
constexpr Frame<n> Reverse<n>::current () {
  return state->at(this->end - this->start);
}

template <size_t n>
constexpr Frame<n> Reverse<n>::at (float t) {
  return state->at(this->end - clamp(t, this->start, this->end));
}
