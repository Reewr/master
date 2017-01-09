namespace __Frame_detail {

  template <size_t n>
  void recursive_ostream (std::ostream& os, const Frame<n>& f) {
    os << f.x << ", ";
    recursive_ostream(os, f.xs);
  }
  template <>
  void recursive_ostream (std::ostream& os, const Frame<1>& f);

  template <size_t n>
  void recursive_print (const Frame<n>& f) {
    std::cout << f.x << ", ";
    recursive_print(f.xs);
  }
  template <>
  void recursive_print (const Frame<1>& f);
}

template <size_t n> template <typename...floats>
constexpr Frame<n>::Frame (float x, float y, floats...xs) : x(x), xs{y, (float)xs...} {}
template <size_t n>
constexpr Frame<n>::Frame (float x, const Frame<n-1>& xs) : x(x), xs(xs) {}
template <size_t n>
constexpr Frame<n>::Frame (float x, Frame<n-1>&& xs) : x(x), xs(std::move(xs)) {}
template <size_t n>
constexpr Frame<n>::Frame (float x) : x(x), xs(0) {}
template <size_t n>
constexpr Frame<n>::Frame () : x(0), xs(0) {}
constexpr Frame<1>::Frame (float x) : x(x) {}
constexpr Frame<1>::Frame () : x(0) {}

template <size_t n>
Frame<n>::Frame (const std::vector<float>& g) {
  float* f = (float*)this;
  for (size_t i=0; i<n; ++i)
    f[i] = i >= g.size() ? 0 : g[i];
}


template <size_t n>
constexpr float Frame<n>::operator [] (size_t i) {
  return ((float*)this)[i];
}
constexpr float Frame<1>::operator [] (size_t i) {
  return ((float*)this)[i];
}

template <size_t n>
float& Frame<n>::operator [] (size_t i) {
  return ((float*)this)[i];
}



template <size_t n>
constexpr Frame<n> operator + (float s, const Frame<n>& g) {
  return Frame<n>(s + g.x, s + g.xs);
}
constexpr Frame<1> operator + (float s, const Frame<1>& g) {
  return Frame<1>(s + g.x);
}

template <size_t n>
constexpr Frame<n> operator + (const Frame<n>& f, float s) {
  return Frame<n>(f.x + s, f.xs + s);
}
constexpr Frame<1> operator + (const Frame<1>& f, float s) {
  return Frame<1>(f.x + s);
}

template <size_t n>
constexpr Frame<n> operator + (const Frame<n>& f, const Frame<n>& g) {
  return Frame<n>(f.x + g.x, f.xs + g.xs);
}
constexpr Frame<1> operator + (const Frame<1>& f, const Frame<1>& g) {
  return Frame<1>(f.x + g.x);
}


template <size_t n>
constexpr Frame<n> operator - (const Frame<n>& f) {
  return Frame<n>(-f.x, -f.xs);
}
constexpr Frame<1> operator - (const Frame<1>& f) {
  return Frame<1>(-f.x);
}

template <size_t n>
constexpr Frame<n> operator - (float s, const Frame<n>& g) {
  return Frame<n>(s - g.x, s - g.xs);
}
constexpr Frame<1> operator - (float s, const Frame<1>& g) {
  return Frame<1>(s - g.x);
}

template <size_t n>
constexpr Frame<n> operator - (const Frame<n>& f, float s) {
  return Frame<n>(f.x - s, f.xs - s);
}
constexpr Frame<1> operator - (const Frame<1>& f, float s) {
  return Frame<1>(f.x - s);
}

template <size_t n>
constexpr Frame<n> operator - (const Frame<n>& f, const Frame<n>& g) {
  return Frame<n>(f.x - g.x, f.xs - g.xs);
}
constexpr Frame<1> operator - (const Frame<1>& f, const Frame<1>& g) {
  return Frame<1>(f.x - g.x);
}


template <size_t n>
constexpr Frame<n> operator * (float s, const Frame<n>& g) {
  return Frame<n>(s * g.x, s * g.xs);
}
constexpr Frame<1> operator * (float s, const Frame<1>& g) {
  return Frame<1>(s * g.x);
}

template <size_t n>
constexpr Frame<n> operator * (const Frame<n>& f, float s) {
  return Frame<n>(f.x * s, f.xs * s);
}
constexpr Frame<1> operator * (const Frame<1>& f, float s) {
  return Frame<1>(f.x * s);
}

template <size_t n>
constexpr Frame<n> operator * (const Frame<n>& f, const Frame<n>& g) {
  return Frame<n>(f.x * g.x, f.xs * g.xs);
}
constexpr Frame<1> operator * (const Frame<1>& f, const Frame<1>& g) {
  return Frame<1>(f.x * g.x);
}


template <size_t n>
constexpr Frame<n> operator / (float s, const Frame<n>& g) {
  return Frame<n>(s / g.x, s / g.xs);
}
constexpr Frame<1> operator / (float s, const Frame<1>& g) {
  return Frame<1>(s / g.x);
}

template <size_t n>
constexpr Frame<n> operator / (const Frame<n>& f, float s) {
  return Frame<n>(f.x / s, f.xs / s);
}
constexpr Frame<1> operator / (const Frame<1>& f, float s) {
  return Frame<1>(f.x / s);
}

template <size_t n>
constexpr Frame<n> operator / (const Frame<n>& f, const Frame<n>& g) {
  return Frame<n>(f.x / g.x, f.xs / g.xs);
}
constexpr Frame<1> operator / (const Frame<1>& f, const Frame<1>& g) {
  return Frame<1>(f.x / g.x);
}


template <size_t n>
constexpr bool operator == (const Frame<n>& f, const Frame<n>& g) {
  return f.x == g.x and f.xs == g.xs;
}
constexpr bool operator == (const Frame<1>& f, const Frame<1>& g) {
  return f.x == g.x;
}

template <size_t n>
constexpr bool operator != (const Frame<n>& f, const Frame<n>& g) {
  return f.x != g.x and f.xs == g.xs;
}
constexpr bool operator != (const Frame<1>& f, const Frame<1>& g) {
  return f.x != g.x;
}


template <size_t n>
Frame<n>& Frame<n>::operator += (float s) {
  float* f = (float*)this;

  for (size_t i=0; i<n; ++i)
    f[i] += s;

  return *this;
}
template <size_t n>
Frame<n>& Frame<n>::operator += (const Frame<n>& g) {
  float* fs = (float*)this;
  float* gs = (float*)&g;

  for (size_t i=0; i<n; ++i)
    fs[i] += gs[i];

  return *this;
}

template <size_t n>
Frame<n>& Frame<n>::operator -= (float s) {
  float* f = (float*)this;

  for (size_t i=0; i<n; ++i)
    f[i] -= s;

  return *this;
}
template <size_t n>
Frame<n>& Frame<n>::operator -= (const Frame<n>& g) {
  float* fs = (float*)this;
  float* gs = (float*)&g;

  for (size_t i=0; i<n; ++i)
    fs[i] -= gs[i];

  return *this;
}

template <size_t n>
Frame<n>& Frame<n>::operator *= (float s) {
  float* f = (float*)this;

  for (size_t i=0; i<n; ++i)
    f[i] *= s;

  return *this;
}
template <size_t n>
Frame<n>& Frame<n>::operator *= (const Frame<n>& g) {
  float* fs = (float*)this;
  float* gs = (float*)&g;

  for (size_t i=0; i<n; ++i)
    fs[i] *= gs[i];

  return *this;
}

template <size_t n>
Frame<n>& Frame<n>::operator /= (float s) {
  float* f = (float*)this;

  for (size_t i=0; i<n; ++i)
    f[i] /= s;

  return *this;
}
template <size_t n>
Frame<n>& Frame<n>::operator /= (const Frame<n>& g) {
  float* fs = (float*)this;
  float* gs = (float*)&g;

  for (size_t i=0; i<n; ++i)
    fs[i] /= gs[i];

  return *this;
}


template <size_t n>
std::ostream& operator << (std::ostream& os, const Frame<n>& f) {
  os << "[";
  __Frame_detail::recursive_ostream(os, f);
  return os;
}

template <size_t n>
void print (const Frame<n>& f) {
  std::cout << "[";
  __Frame_detail::recursive_print(f);
}
