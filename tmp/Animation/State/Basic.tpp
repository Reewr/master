template <size_t n>
constexpr Basic<n>::Basic (Data<n> const * const data) :
  State<n>(0, data->length()), data(data) {}

template <size_t n>
constexpr Basic<n>::Basic (Data<n> const * const data, float start, float end) :
  State<n>(start, end), data(data) {}

template <size_t n>
constexpr State<n>* Basic<n>::copy () {
  return new Basic<n>(data, this->start, this->end);
}

template <size_t n>
constexpr Frame<n> Basic<n>::current () {
  return data->interpolate(this->start);
}

template <size_t n>
constexpr Frame<n> Basic<n>::at (float t) {
  return data->interpolate(clamp(t, this->start, this->end));
}
