template <size_t n>
constexpr Sequence<n>::Sequence (
  State<n>*const state1, State<n>*const state2) :
    State<n>(0, state1->length() + state2->length()),
    state1(state1), state2(state2) {}

template <size_t n>
constexpr Sequence<n>::Sequence (
  State<n>*const state1, State<n>*const state2, float start, float end) :
    State<n>(start, end), state1(state1), state2(state2) {}


template <size_t n>
constexpr State<n>* Sequence<n>::copy () {
  return new Sequence<n>(state1->copy(), state2->copy(), this->start, this->end);
}

template <size_t n>
constexpr Frame<n> Sequence<n>::current () {
  return this->start < state1->length()
    ? state1->at(this->start)
    : state2->at(this->start - state1->length());
}

template <size_t n>
constexpr Frame<n> Sequence<n>::at (float t) {
  return t < state1->length()
    ? state1->at(clamp(t, 0, state1->length()))
    : state2->at(clamp(t - state1->length(), 0, state2->length()));
}
