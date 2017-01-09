template <size_t n>
constexpr Transition<n>::Transition (
  State<n>*const state1, State<n>*const state2) :
    State<n>(0, fmax(state1->length(), state2->length())),
    state1(state1), state2(state2) {}

template <size_t n>
constexpr Transition<n>::Transition (
  State<n>*const state1, State<n>*const state2, float start, float end) :
    State<n>(start, end), state1(state1), state2(state2) {}

template <size_t n>
constexpr State<n>* Transition<n>::copy () {
  return new Transition<n>(state1->copy(), state2->copy(), this->start, this->end);
}

template <size_t n>
constexpr Frame<n> Transition<n>::current () {
  return state1->at(this->start) + (this->start / this->end) *
    (state2->at(this->start) - state1->at(this->start));
}

template <size_t n>
constexpr Frame<n> Transition<n>::at (float t) {
  return _at (clamp (t, this->start, this->end));
}

template <size_t n>
constexpr Frame<n> Transition<n>::_at (float t) {
  return state1->at(t) + (t / this->end) *
    (state2->at(t) - state1->at(t));
}
