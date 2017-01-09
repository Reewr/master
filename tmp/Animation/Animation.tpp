template <size_t n>
constexpr Reverse<n>::Reverse (Anim<n>&& anim) :
  State<n>(0, anim.state->length()), state(std::move(anim.state)) {}

template <size_t n>
constexpr Reverse<n>::Reverse (const Anim<n>& anim) :
  State<n>(0, anim.state->length()), state(anim.state->copy()) {}


template <size_t n>
constexpr Loop<n>::Loop (Anim<n>&& anim) :
  State<n>(0, anim.state->length()), acc(0), state(std::move(anim.state)) {}

template <size_t n>
constexpr Loop<n>::Loop (const Anim<n>& anim) :
  State<n>(0, anim.state->length()), acc(0), state(anim.state->copy()) {}


template <size_t n>
constexpr Blend<n,State<n>>::Blend (
  Func blend, Anim<n>&& anim) :
    State<n>(0, anim.state->length()),
    blend(blend), state(std::move(anim.state)) {}

template <size_t n>
constexpr Blend<n,State<n>>::Blend (
  Func blend, const Anim<n>& anim) :
    State<n>(0, anim.state->length()),
    blend(blend), state(anim.state->copy()) {}


template <size_t n>
constexpr Blend<n,float,State<n>>::Blend (
  Func blend, float s, Anim<n>&& anim) :
    State<n>(0, anim.state->length()),
    blend(blend), scalar(s), state(std::move(anim.state)) {}

template <size_t n>
constexpr Blend<n,float,State<n>>::Blend (
  Func blend, float s, const Anim<n>& anim) :
    State<n>(0, anim.state->length()),
    blend(blend), scalar(s), state(anim.state->copy()) {}

template <size_t n>
constexpr Blend<n,Var,State<n>>::Blend (
  Func blend, Var s, Anim<n>&& anim) :
    State<n>(0, anim.state->length()),
    blend(blend), scalar(s), state(std::move(anim.state)) {}

template <size_t n>
constexpr Blend<n,Var,State<n>>::Blend (
  Func blend, Var s, const Anim<n>& anim) :
    State<n>(0, anim.state->length()),
    blend(blend), scalar(s), state(anim.state->copy()) {}


template <size_t n>
constexpr Blend<n,State<n>,float>::Blend (
  Func blend, Anim<n>&& anim, float s) :
    State<n>(0, anim.state->length()),
    blend(blend), state(std::move(anim.state)), scalar(s) {}

template <size_t n>
constexpr Blend<n,State<n>,float>::Blend (
  Func blend, const Anim<n>& anim, float s) :
    State<n>(0, anim.state->length()),
    blend(blend), state(anim.state->copy()), scalar(s) {}

template <size_t n>
constexpr Blend<n,State<n>,Var>::Blend (
  Func blend, Anim<n>&& anim, Var s) :
    State<n>(0, anim.state->length()),
    blend(blend), state(std::move(anim.state)), scalar(s) {}

template <size_t n>
constexpr Blend<n,State<n>,Var>::Blend (
  Func blend, const Anim<n>& anim, Var s) :
    State<n>(0, anim.state->length()),
    blend(blend), state(anim.state->copy()), scalar(s) {}


template <size_t n>
constexpr Blend<n,State<n>,State<n>>::Blend (
  Func blend, Anim<n>&& anim1, Anim<n>&& anim2) :
    State<n>(0, fmax(anim1.state->length(), anim2.state->length())),
    blend(blend), state1(std::move(anim1.state)),
    state2(std::move(anim2.state)) {}

template <size_t n>
constexpr Blend<n,State<n>,State<n>>::Blend (
  Func blend, const Anim<n>& anim1, const Anim<n>& anim2) :
    State<n>(0, fmax(anim1.state->length(), anim2.state->length())),
    blend(blend), state1(anim1.state->copy()), state2(anim2.state->copy()) {}


template <size_t n>
constexpr Eval<n>::Eval (Var v, Anim<n>&& anim) :
  State<n>(0, anim.state->length()), var(v), state(std::move(anim.state)) {}

template <size_t n>
constexpr Eval<n>::Eval (Var v, const Anim<n>& anim) :
  State<n>(0, anim.state->length()), var(v), state(anim.state->copy()) {}


template <size_t n>
constexpr Sequence<n>::Sequence (Anim<n>&& anim1, Anim<n>&& anim2) :
  State<n>(0, anim1.state->length() + anim2.state->length()),
  state1(std::move(anim1.state)), state2(std::move(anim2.state)) {}

template <size_t n>
constexpr Sequence<n>::Sequence (const Anim<n>& anim1, const Anim<n>& anim2) :
  State<n>(0, anim1.state->length() + anim2.state->length()),
  state1(anim1.state->copy()), state2(anim2.state->copy()) {}


template <size_t n>
constexpr Transition<n>::Transition (Anim<n>&& anim1, Anim<n>&& anim2) :
  State<n>(0, fmax(anim1.state->length(), anim2.state->length())),
  state1(std::move(anim1.state)), state2(std::move(anim2.state)) {}

template <size_t n>
constexpr Transition<n>::Transition (const Anim<n>& anim1, const Anim<n>& anim2) :
  State<n>(0, fmax(anim1.state->length(), anim2.state->length())),
  state1(anim1.state->copy()), state2(anim2.state->copy()) {}


template <size_t n>
constexpr Anim<n>::Anim () :
  state(new Pose<n>(Frame<n>{0})) {}

template <size_t n>
constexpr Anim<n>::Anim (Frame<n>&& frame) :
  state(new Pose<n>(std::move(frame))) {}

template <size_t n>
constexpr Anim<n>::Anim (const Frame<n>& frame) :
  state(new Pose<n>(frame)) {}

template <size_t n>
constexpr Anim<n>::Anim (Data<n> const * const data) :
  state(new Basic<n>(data)) {}

template <size_t n>
constexpr Anim<n>::Anim (State<n>* state) :
  state(state) {}

template <size_t n>
constexpr Anim<n>::Anim (Anim<n>&& b) :
  state(std::move(b.state)) {}

template <size_t n>
constexpr Anim<n>::Anim (const Anim<n>& b) :
  state(b.state->copy()) {}



template <size_t n>
constexpr Frame<n> Anim<n>::operator () (float dt) {
  return state->advance(dt)->current();
}
template <size_t n>
constexpr Frame<n> Anim<n>::operator [] (float t) {
  return state->at(t);
}
template <size_t n>
constexpr Anim<n>  Anim<n>::operator [] (Var v) {
  return new Eval<n>(v, *this);
}


template <size_t n>
Anim<n>& Anim<n>::operator >= (Anim<n>&& b) {
  state.reset(new Sequence<n>(std::move(*this), std::move(b)));
  return *this;
}
template <size_t n>
Anim<n>& Anim<n>::operator >= (const Anim<n>& b) {
  state.reset(new Sequence<n>(std::move(*this), b));
  return *this;
}




template <size_t n>
Anim<n>& Anim<n>::operator += (float s) {
  state.reset(new Blend<n,State<n>,float>(
    Blends::add, std::move(*this), s)
  );
  return *this;
}
template <size_t n>
Anim<n>& Anim<n>::operator += (Var s) {
  state.reset(new Blend<n,State<n>,Var>(
    Blends::add, std::move(*this), s)
  );
  return *this;
}
template <size_t n>
Anim<n>& Anim<n>::operator += (Anim<n>&& b) {
  state.reset(new Blend<n,State<n>,State<n>>(
    Blends::add, std::move(*this), std::move(b))
  );
  return *this;
}
template <size_t n>
Anim<n>& Anim<n>::operator += (const Anim<n>& b) {
  state.reset(new Blend<n,State<n>,State<n>>(
    Blends::add, std::move(*this), b)
  );
  return *this;
}

template <size_t n>
Anim<n>& Anim<n>::operator -= (float s) {
  state.reset(new Blend<n,State<n>,float>(
    Blends::sub, std::move(*this), s)
  );
  return *this;
}
template <size_t n>
Anim<n>& Anim<n>::operator -= (Var s) {
  state.reset(new Blend<n,State<n>,Var>(
    Blends::sub, std::move(*this), s)
  );
  return *this;
}
template <size_t n>
Anim<n>& Anim<n>::operator -= (Anim<n>&& b) {
  state.reset(new Blend<n,State<n>,State<n>>(
    Blends::sub, std::move(*this), std::move(b))
  );
  return *this;
}
template <size_t n>
Anim<n>& Anim<n>::operator -= (const Anim<n>& b) {
  state.reset(new Blend<n,State<n>,State<n>>(
    Blends::sub, std::move(*this), b)
  );
  return *this;
}

template <size_t n>
Anim<n>& Anim<n>::operator *= (float s) {
  state.reset(new Blend<n,State<n>,float>(
    Blends::mult, std::move(*this), s)
  );
  return *this;
}
template <size_t n>
Anim<n>& Anim<n>::operator *= (Var s) {
  state.reset(new Blend<n,State<n>,Var>(
    Blends::mult, std::move(*this), s)
  );
  return *this;
}
template <size_t n>
Anim<n>& Anim<n>::operator *= (Anim<n>&& b) {
  state.reset(new Blend<n,State<n>,State<n>>(
    Blends::mult, std::move(*this), std::move(b))
  );
  return *this;
}
template <size_t n>
Anim<n>& Anim<n>::operator *= (const Anim<n>& b) {
  state.reset(new Blend<n,State<n>,State<n>>(
    Blends::mult, std::move(*this), b)
  );
  return *this;
}

template <size_t n>
Anim<n>& Anim<n>::operator /= (float s) {
  state.reset(new Blend<n,State<n>,float>(
    Blends::div, std::move(*this), s)
  );
  return *this;
}
template <size_t n>
Anim<n>& Anim<n>::operator /= (Var s) {
  state.reset(new Blend<n,State<n>,Var>(
    Blends::div, std::move(*this), s)
  );
  return *this;
}
template <size_t n>
Anim<n>& Anim<n>::operator /= (Anim<n>&& b) {
  state.reset(new Blend<n,State<n>,State<n>>(
    Blends::div, std::move(*this), std::move(b))
  );
  return *this;
}
template <size_t n>
Anim<n>& Anim<n>::operator /= (const Anim<n>& b) {
  state.reset(new Blend<n,State<n>,State<n>>(
    Blends::div, std::move(*this), b)
  );
  return *this;
}



template <size_t n>
Anim<n>& Anim<n>::operator = (Anim<n>&& b) {
  state = std::move(b.state);
  return *this;
}
template <size_t n>
Anim<n>& Anim<n>::operator = (const Anim<n>& b) {
  state.reset(b.state->copy());
  return *this;
}



template <size_t n>
Data<n>* Anim<n>::compile (int frames) {
  return state->compile(frames);
}


template <size_t n>
constexpr Anim<n> transition (Anim<n>&& a, Anim<n>&& b) {
  return new Transition<n>(std::move(a), std::move(b));
}
template <size_t n>
constexpr Anim<n> transition (Anim<n>&& a, const Anim<n>& b) {
  return new Transition<n>(std::move(a), b);
}
template <size_t n>
constexpr Anim<n> transition (const Anim<n>& a, Anim<n>&& b) {
  return new Transition<n>(a, std::move(b));
}
template <size_t n>
constexpr Anim<n> transition (const Anim<n>& a, const Anim<n>& b) {
  return new Transition<n>(a, b);
}


template <size_t n>
constexpr Anim<n> loop (Anim<n>&& a) {
  return new Loop<n>(std::move(a));
}
template <size_t n>
constexpr Anim<n> loop (const Anim<n>& a) {
  return new Loop<n>(a);
}

template <size_t n>
constexpr Anim<n> operator > (Anim<n>&& a, Anim<n>&& b) {
  return new Sequence<n>(std::move(a), std::move(b));
}
template <size_t n>
constexpr Anim<n> operator > (Anim<n>&& a, const Anim<n>& b) {
  return new Sequence<n>(std::move(a), b);
}
template <size_t n>
constexpr Anim<n> operator > (const Anim<n>& a, Anim<n>&& b) {
  return new Sequence<n>(a, std::move(b));
}
template <size_t n>
constexpr Anim<n> operator > (const Anim<n>& a, const Anim<n>& b) {
  return new Sequence<n>(a, b);
}



template <size_t n>
constexpr Anim<n> operator + (float s, Anim<n>&& b) {
  return new Blend<n,float,State<n>>(Blends::add, s, std::move(b));
}
template <size_t n>
constexpr Anim<n> operator + (float s, const Anim<n>& b) {
  return new Blend<n,float,State<n>>(Blends::add, s, b);
}
template <size_t n>
constexpr Anim<n> operator + (Var s, Anim<n>&& b) {
  return new Blend<n,Var,State<n>>(Blends::add, s, std::move(b));
}
template <size_t n>
constexpr Anim<n> operator + (Var s, const Anim<n>& b) {
  return new Blend<n,Var,State<n>>(Blends::add, s, b);
}
template <size_t n>
constexpr Anim<n> operator + (Anim<n>&& a, float s) {
  return new Blend<n,State<n>,float>(Blends::add, std::move(a), s);
}
template <size_t n>
constexpr Anim<n> operator + (const Anim<n>& a, float s) {
  return new Blend<n,State<n>,float>(Blends::add, a ,s);
}
template <size_t n>
constexpr Anim<n> operator + (Anim<n>&& a, Var s) {
  return new Blend<n,State<n>,Var>(Blends::add, std::move(a), s);
}
template <size_t n>
constexpr Anim<n> operator + (const Anim<n>& a, Var s) {
  return new Blend<n,State<n>,Var>(Blends::add, a, s);
}
template <size_t n>
constexpr Anim<n> operator + (Anim<n>&& a, Anim<n>&& b) {
  return new Blend<n,State<n>,State<n>>(Blends::add, std::move(a), std::move(b));
}
template <size_t n>
constexpr Anim<n> operator + (Anim<n>&& a, const Anim<n>& b) {
  return new Blend<n,State<n>,State<n>>(Blends::add, std::move(a), b);
}
template <size_t n>
constexpr Anim<n> operator + (const Anim<n>& a, Anim<n>&& b) {
  return new Blend<n,State<n>,State<n>>(Blends::add, a, std::move(b));
}
template <size_t n>
constexpr Anim<n> operator + (const Anim<n>& a, const Anim<n>& b) {
  return new Blend<n,State<n>,State<n>>(Blends::add, a, b);
}

template <size_t n>
constexpr Anim<n> operator - (Anim<n>&& a) {
  return new Blend<n,State<n>>(Blends::neg, std::move(a));
}
template <size_t n>
constexpr Anim<n> operator - (const Anim<n>& a) {
  return new Blend<n,State<n>>(Blends::neg, a);
}

template <size_t n>
constexpr Anim<n> operator - (float s, Anim<n>&& b) {
  return new Blend<n,float,State<n>>(Blends::sub, s, std::move(b));
}
template <size_t n>
constexpr Anim<n> operator - (float s, const Anim<n>& b) {
  return new Blend<n,float,State<n>>(Blends::sub, s, b);
}
template <size_t n>
constexpr Anim<n> operator - (Var s, Anim<n>&& b) {
  return new Blend<n,Var,State<n>>(Blends::sub, s, std::move(b));
}
template <size_t n>
constexpr Anim<n> operator - (Var s, const Anim<n>& b) {
  return new Blend<n,Var,State<n>>(Blends::sub, s, b);
}
template <size_t n>
constexpr Anim<n> operator - (Anim<n>&& a, float s) {
  return new Blend<n,State<n>,float>(Blends::sub, std::move(a), s);
}
template <size_t n>
constexpr Anim<n> operator - (const Anim<n>& a, float s) {
  return new Blend<n,State<n>,float>(Blends::sub, a ,s);
}
template <size_t n>
constexpr Anim<n> operator - (Anim<n>&& a, Var s) {
  return new Blend<n,State<n>,Var>(Blends::sub, std::move(a), s);
}
template <size_t n>
constexpr Anim<n> operator - (const Anim<n>& a, Var s) {
  return new Blend<n,State<n>,Var>(Blends::sub, a ,s);
}
template <size_t n>
constexpr Anim<n> operator - (Anim<n>&& a, Anim<n>&& b) {
  return new Blend<n,State<n>,State<n>>(Blends::sub, std::move(a), std::move(b));
}
template <size_t n>
constexpr Anim<n> operator - (Anim<n>&& a, const Anim<n>& b) {
  return new Blend<n,State<n>,State<n>>(Blends::sub, std::move(a), b);
}
template <size_t n>
constexpr Anim<n> operator - (const Anim<n>& a, Anim<n>&& b) {
  return new Blend<n,State<n>,State<n>>(Blends::sub, a, std::move(b));
}
template <size_t n>
constexpr Anim<n> operator - (const Anim<n>& a, const Anim<n>& b) {
  return new Blend<n,State<n>,State<n>>(Blends::sub, a, b);
}

template <size_t n>
constexpr Anim<n> operator * (float s, Anim<n>&& b) {
  return new Blend<n,float,State<n>>(Blends::mult, s, std::move(b));
}
template <size_t n>
constexpr Anim<n> operator * (float s, const Anim<n>& b) {
  return new Blend<n,float,State<n>>(Blends::mult, s, b);
}
template <size_t n>
constexpr Anim<n> operator * (Var s, Anim<n>&& b) {
  return new Blend<n,Var,State<n>>(Blends::mult, s, std::move(b));
}
template <size_t n>
constexpr Anim<n> operator * (Var s, const Anim<n>& b) {
  return new Blend<n,Var,State<n>>(Blends::mult, s, b);
}
template <size_t n>
constexpr Anim<n> operator * (Anim<n>&& a, float s) {
  return new Blend<n,State<n>,float>(Blends::mult, std::move(a), s);
}
template <size_t n>
constexpr Anim<n> operator * (const Anim<n>& a, float s) {
  return new Blend<n,State<n>,float>(Blends::mult, a ,s);
}
template <size_t n>
constexpr Anim<n> operator * (Anim<n>&& a, Var s) {
  return new Blend<n,State<n>,Var>(Blends::mult, std::move(a), s);
}
template <size_t n>
constexpr Anim<n> operator * (const Anim<n>& a, Var s) {
  return new Blend<n,State<n>,Var>(Blends::mult, a ,s);
}
template <size_t n>
constexpr Anim<n> operator * (Anim<n>&& a, Anim<n>&& b) {
  return new Blend<n,State<n>,State<n>>(Blends::mult, std::move(a), std::move(b));
}
template <size_t n>
constexpr Anim<n> operator * (Anim<n>&& a, const Anim<n>& b) {
  return new Blend<n,State<n>,State<n>>(Blends::mult, std::move(a), b);
}
template <size_t n>
constexpr Anim<n> operator * (const Anim<n>& a, Anim<n>&& b) {
  return new Blend<n,State<n>,State<n>>(Blends::mult, a, std::move(b));
}
template <size_t n>
constexpr Anim<n> operator * (const Anim<n>& a, const Anim<n>& b) {
  return new Blend<n,State<n>,State<n>>(Blends::mult, a, b);
}

template <size_t n>
constexpr Anim<n> operator / (float s, Anim<n>&& b) {
  return new Blend<n,float,State<n>>(Blends::div, s, std::move(b));
}
template <size_t n>
constexpr Anim<n> operator / (float s, const Anim<n>& b) {
  return new Blend<n,float,State<n>>(Blends::div, s, b);
}
template <size_t n>
constexpr Anim<n> operator / (Var s, Anim<n>&& b) {
  return new Blend<n,Var,State<n>>(Blends::div, s, std::move(b));
}
template <size_t n>
constexpr Anim<n> operator / (Var s, const Anim<n>& b) {
  return new Blend<n,Var,State<n>>(Blends::div, s, b);
}
template <size_t n>
constexpr Anim<n> operator / (Anim<n>&& a, float s) {
  return new Blend<n,State<n>,float>(Blends::div, std::move(a), s);
}
template <size_t n>
constexpr Anim<n> operator / (const Anim<n>& a, float s) {
  return new Blend<n,State<n>,float>(Blends::div, a ,s);
}
template <size_t n>
constexpr Anim<n> operator / (Anim<n>&& a, Var s) {
  return new Blend<n,State<n>,Var>(Blends::div, std::move(a), s);
}
template <size_t n>
constexpr Anim<n> operator / (const Anim<n>& a, Var s) {
  return new Blend<n,State<n>,Var>(Blends::div, a ,s);
}
template <size_t n>
constexpr Anim<n> operator / (Anim<n>&& a, Anim<n>&& b) {
  return new Blend<n,State<n>,State<n>>(Blends::div, std::move(a), std::move(b));
}
template <size_t n>
constexpr Anim<n> operator / (Anim<n>&& a, const Anim<n>& b) {
  return new Blend<n,State<n>,State<n>>(Blends::div, std::move(a), b);
}
template <size_t n>
constexpr Anim<n> operator / (const Anim<n>& a, Anim<n>&& b) {
  return new Blend<n,State<n>,State<n>>(Blends::div, a, std::move(b));
}
template <size_t n>
constexpr Anim<n> operator / (const Anim<n>& a, const Anim<n>& b) {
  return new Blend<n,State<n>,State<n>>(Blends::div, a, b);
}
