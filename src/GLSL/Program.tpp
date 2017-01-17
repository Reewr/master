template <typename T>
bool Program::setUniform(const std::string& uni, const T& t) {
  bind();
  GLint loc = getUniformLocation(uni);
  if (loc != -1)
    return setGLUniform(loc, t);
  return false;
}

template <typename T>
bool Program::setUniforms(const std::vector<std::string>& unis,
                          const std::vector<T>&           t) {
  if (unis.size() != t.size()) {
    error("setUniforms: size is not equal");
    return false;
  }

  for (unsigned int i = 0; i < unis.size(); i++) {
    if (!setUniform(unis[i], t[i]))
      return false;
  }
  return true;
}

template <typename T, typename... Ts>
bool Program::setUniform(const std::string& uni, T x, Ts... xs) {
  setUniform(uni, x);
  return setUniform(xs...);
}
