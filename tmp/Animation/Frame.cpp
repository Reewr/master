#include <Animation/Frame.hpp>

namespace Animation {

  template <>
  void __Frame_detail::recursive_ostream (std::ostream& os, const Frame<1>& f) {
    os << f.x << "]";
  }

  template <>
  void __Frame_detail::recursive_print (const Frame<1>& f) {
    std::cout << f.x << "]" << std::endl;
  }

  Frame<1>::Frame (const std::vector<float>& g) {
    x = g.size() < 1 ? 0 : g[0];
  }

  float& Frame<1>::operator [] (size_t i) {
    return ((float*)this)[i];
  }


  Frame<1>& Frame<1>::operator += (float s) {
    x += s;
    return *this;
  }
  Frame<1>& Frame<1>::operator += (const Frame<1>& g) {
    x += g.x;
    return *this;
  }

  Frame<1>& Frame<1>::operator -= (float s) {
    x -= s;
    return *this;
  }
  Frame<1>& Frame<1>::operator -= (const Frame<1>& g) {
    x -= g.x;
    return *this;
  }

  Frame<1>& Frame<1>::operator *= (float s) {
    x *= s;
    return *this;
  }
  Frame<1>& Frame<1>::operator *= (const Frame<1>& g) {
    x *= g.x;
    return *this;
  }

  Frame<1>& Frame<1>::operator /= (float s) {
    x /= s;
    return *this;
  }
  Frame<1>& Frame<1>::operator /= (const Frame<1>& g) {
    x /= g.x;
    return *this;
  }
}
