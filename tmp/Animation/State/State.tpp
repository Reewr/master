template <size_t n>
constexpr State<n>::State (float start, float end) : start(start), end(end) {}

template <size_t n>
State<n>::~State () {}


template <size_t n>
constexpr float State<n>::length () {
  return end - start;
}

template <size_t n>
State<n>* State<n>::advance (float dt) {
  this->start = clamp (this->start + dt, this->start, this->end);
  return this;
}

template <size_t n>
Data<n>* State<n>::compile (int frames) {
  std::vector<KeyFrame<n>> fs;
  float step = this->length() / frames;

  for (int i = 0; i < frames; ++i)
    fs.push_back(KeyFrame<n>(step*i, this->at(step * i)));

  return new Data<n>(fs);
}

// pure virtual constexpr function used but never defined.
// this is a hack to stop g++ giving the above warning.
  template <size_t n>
  constexpr State<n>* State<n>::copy ()    {throw "STFU g++ you are drunk!";}

  template <size_t n>
  constexpr Frame<n>  State<n>::current () {throw "STFU g++ you are drunk!";}

  template <size_t n>
  constexpr Frame<n>  State<n>::at (float) {throw "STFU g++ you are drunk!";}
