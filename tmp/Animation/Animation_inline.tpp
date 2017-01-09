friend constexpr Anim<n> operator > (Anim<n>&& a, Anim<n>&& b) {
  return new Sequence<n>(std::move(a), std::move(b));
}
friend constexpr Anim<n> operator > (Anim<n>&& a, const Anim<n>& b) {
  return new Sequence<n>(std::move(a), b);
}
friend constexpr Anim<n> operator > (const Anim<n>& a, Anim<n>&& b) {
  return new Sequence<n>(a, std::move(b));
}
friend constexpr Anim<n> operator > (const Anim<n>& a, const Anim<n>& b) {
  return new Sequence<n>(a, b);
}

friend constexpr Anim<n> operator + (float s, Anim<n>&& b) {
  return new Blend<n,float,State<n>>(Blends::add, s, std::move(b));
}
friend constexpr Anim<n> operator + (float s, const Anim<n>& b) {
  return new Blend<n,float,State<n>>(Blends::add, s, b);
}
friend constexpr Anim<n> operator + (Anim<n>&& a, float s) {
  return new Blend<n,State<n>,float>(Blends::add, std::move(a), s);
}
friend constexpr Anim<n> operator + (const Anim<n>& a, float s) {
  return new Blend<n,State<n>,float>(Blends::add, a ,s);
}
friend constexpr Anim<n> operator + (Anim<n>&& a, Anim<n>&& b) {
  return new Blend<n,State<n>,State<n>>(Blends::add, std::move(a), std::move(b));
}
friend constexpr Anim<n> operator + (Anim<n>&& a, const Anim<n>& b) {
  return new Blend<n,State<n>,State<n>>(Blends::add, std::move(a), b);
}
friend constexpr Anim<n> operator + (const Anim<n>& a, Anim<n>&& b) {
  return new Blend<n,State<n>,State<n>>(Blends::add, a, std::move(b));
}
friend constexpr Anim<n> operator + (const Anim<n>& a, const Anim<n>& b) {
  return new Blend<n,State<n>,State<n>>(Blends::add, a, b);
}

friend constexpr Anim<n> operator - (Anim<n>&& a) {
  return new Blend<n,State<n>>(Blends::neg, std::move(a));
}
friend constexpr Anim<n> operator - (const Anim<n>& a) {
  return new Blend<n,State<n>>(Blends::neg, a);
}
friend constexpr Anim<n> operator - (float s, Anim<n>&& b) {
  return new Blend<n,float,State<n>>(Blends::sub, s, std::move(b));
}
friend constexpr Anim<n> operator - (float s, const Anim<n>& b) {
  return new Blend<n,float,State<n>>(Blends::sub, s, b);
}
friend constexpr Anim<n> operator - (Anim<n>&& a, float s) {
  return new Blend<n,State<n>,float>(Blends::sub, std::move(a), s);
}
friend constexpr Anim<n> operator - (const Anim<n>& a, float s) {
  return new Blend<n,State<n>,float>(Blends::sub, a ,s);
}
friend constexpr Anim<n> operator - (Anim<n>&& a, Anim<n>&& b) {
  return new Blend<n,State<n>,State<n>>(Blends::sub, std::move(a), std::move(b));
}
friend constexpr Anim<n> operator - (Anim<n>&& a, const Anim<n>& b) {
  return new Blend<n,State<n>,State<n>>(Blends::sub, std::move(a), b);
}
friend constexpr Anim<n> operator - (const Anim<n>& a, Anim<n>&& b) {
  return new Blend<n,State<n>,State<n>>(Blends::sub, a, std::move(b));
}
friend constexpr Anim<n> operator - (const Anim<n>& a, const Anim<n>& b) {
  return new Blend<n,State<n>,State<n>>(Blends::sub, a, b);
}

friend constexpr Anim<n> operator * (float s, Anim<n>&& b) {
  return new Blend<n,float,State<n>>(Blends::mult, s, std::move(b));
}
friend constexpr Anim<n> operator * (float s, const Anim<n>& b) {
  return new Blend<n,float,State<n>>(Blends::mult, s, b);
}
friend constexpr Anim<n> operator * (Anim<n>&& a, float s) {
  return new Blend<n,State<n>,float>(Blends::mult, std::move(a), s);
}
friend constexpr Anim<n> operator * (const Anim<n>& a, float s) {
  return new Blend<n,State<n>,float>(Blends::mult, a ,s);
}
friend constexpr Anim<n> operator * (Anim<n>&& a, Anim<n>&& b) {
  return new Blend<n,State<n>,State<n>>(Blends::mult, std::move(a), std::move(b));
}
friend constexpr Anim<n> operator * (Anim<n>&& a, const Anim<n>& b) {
  return new Blend<n,State<n>,State<n>>(Blends::mult, std::move(a), b);
}
friend constexpr Anim<n> operator * (const Anim<n>& a, Anim<n>&& b) {
  return new Blend<n,State<n>,State<n>>(Blends::mult, a, std::move(b));
}
friend constexpr Anim<n> operator * (const Anim<n>& a, const Anim<n>& b) {
  return new Blend<n,State<n>,State<n>>(Blends::mult, a, b);
}

friend constexpr Anim<n> operator / (float s, Anim<n>&& b) {
  return new Blend<n,float,State<n>>(Blends::div, s, std::move(b));
}
friend constexpr Anim<n> operator / (float s, const Anim<n>& b) {
  return new Blend<n,float,State<n>>(Blends::div, s, b);
}
friend constexpr Anim<n> operator / (Anim<n>&& a, float s) {
  return new Blend<n,State<n>,float>(Blends::div, std::move(a), s);
}
friend constexpr Anim<n> operator / (const Anim<n>& a, float s) {
  return new Blend<n,State<n>,float>(Blends::div, a ,s);
}
friend constexpr Anim<n> operator / (Anim<n>&& a, Anim<n>&& b) {
  return new Blend<n,State<n>,State<n>>(Blends::div, std::move(a), std::move(b));
}
friend constexpr Anim<n> operator / (Anim<n>&& a, const Anim<n>& b) {
  return new Blend<n,State<n>,State<n>>(Blends::div, std::move(a), b);
}
friend constexpr Anim<n> operator / (const Anim<n>& a, Anim<n>&& b) {
  return new Blend<n,State<n>,State<n>>(Blends::div, a, std::move(b));
}
friend constexpr Anim<n> operator / (const Anim<n>& a, const Anim<n>& b) {
  return new Blend<n,State<n>,State<n>>(Blends::div, a, b);
}