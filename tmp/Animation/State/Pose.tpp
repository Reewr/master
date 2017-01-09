template <size_t n>
constexpr Pose<n>::Pose (Frame<n>&& frame) :
  State<n>(0,0), frame(std::move(frame)) {}

template <size_t n>
constexpr Pose<n>::Pose (const Frame<n>& frame) :
  State<n>(0,0), frame(frame) {}


template <size_t n>
constexpr State<n>* Pose<n>::copy () {
  return new Pose<n>(frame);
}

template <size_t n>
constexpr float Pose<n>::length () {
  return 0;
}

template <size_t n>
State<n>* Pose<n>::advance (float) {
  return this;
}

template <size_t n>
constexpr Frame<n> Pose<n>::current () {
  return frame;
}

template <size_t n>
constexpr Frame<n> Pose<n>::at (float) {
  return frame;
}
