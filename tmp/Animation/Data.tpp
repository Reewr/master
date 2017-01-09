template <size_t n>
constexpr KeyFrame<n>::KeyFrame () : t(), f() {}

template <size_t n>
constexpr KeyFrame<n>::KeyFrame (float t, Frame<n>&& f) : t(t), f(std::move(f)) {}

template <size_t n>
constexpr KeyFrame<n>::KeyFrame (float t, const Frame<n>& f) : t(t), f(f) {}

template <size_t n> template <typename...floats>
constexpr KeyFrame<n>::KeyFrame (float t, floats...fs) : t(t), f(fs...) {}


template <size_t n> template <typename...KFS>
constexpr Data<n>::Data (KeyFrame<n>&& f, KFS...fs) : fs{std::move(f), fs...} {}

template <size_t n> template <typename...KFS>
constexpr Data<n>::Data (const KeyFrame<n>& f, KFS...fs) : fs{f, fs...} {}

template <size_t n>
constexpr Data<n>::Data (std::vector<KeyFrame<n>>&& fs) : fs(std::move(fs)) {}

template <size_t n>
constexpr Data<n>::Data (const std::vector<KeyFrame<n>>& fs) : fs(fs) {}

template <size_t n>
Data<n>::Data (const char* filePath) : fs(import(filePath)) {}


template <size_t n>
constexpr float Data<n>::length () {
  return fs[fs.size()-1].t;
}


template <size_t n>
constexpr Frame<n> Data<n>::interpolate (float acc) {
  return interpolate_recursive(acc, fs.size()-1);
}
template <size_t n>
constexpr Frame<n> Data<n>::interpolate_recursive (float acc, size_t i) {
  return i == 0 
    ? fs[i].f
    : acc >= fs[i].t
      ? fs[i].f
      : acc > fs[i-1].t
        ? fs[i-1].f + (fs[i].f - fs[i-1].f) * ((acc - fs[i-1].t) / (fs[i].t - fs[i-1].t))
        : interpolate_recursive(acc, i-1);
}


template <size_t n>
std::vector<KeyFrame<n>> Data<n>::import (const char* filePath) {
  Import::Animation a(filePath);
  if (a.n != n) throw Error("unexpected frame size in "+std::string(filePath));

  std::vector<KeyFrame<n>> fs;
  fs.resize(a.l);

  float* _fs = (float*)(&fs[0]);
  for (int i = 0; i < (a.n+1)*a.l; ++i)
    _fs[i] = a.fs[i];

  return fs;
}


template <size_t n>
void print (const KeyFrame<n>& f) {
  std::cout << std::setw(8) << f.t << " - " << f.f << std::endl;
}

template <size_t n>
void print (const Data<n>& d) {
  for (const auto& f: d.fs)
    print (f);
}
