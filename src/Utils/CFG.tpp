static void
unknown_parameter(const Prop& p, const Param& pm, const std::string& v);

int get_num_params(const Prop& p);

struct Wrapper {
  std::function<void(const Prop& p, const Params&)> parse;
  std::function<std::string()> show;
  std::function<std::string()> asType;

  std::string  valid_params = "unknown";
  unsigned int args         = 1;

  // current supported types:
  Wrapper(bool& b);
  Wrapper(int& i);
  Wrapper(float& f);
  Wrapper(mmm::vec2& v);
  Wrapper(ActB& ab);

  // can support any type if it has an assosiated posible-value-map
  template <typename T>
  Wrapper(T& t, std::map<std::string, T> m) {

    std::string str = "";
    for (const auto& kv : m)
      str += " " + kv.first;
    valid_params = str;

    parse = [&t, m, str](const Prop& p, const Params& ps) {
      try {
        t = m.at(ps[0]);
      } catch (...) {
        unknown_parameter(p, ps[0], str);
      }
    };

    show = [&t, m]() {
      for (const auto& s : m)
        if (t == s.second)
          return s.first;
      return std::string();
    };
  }

  void add_special_case(std::function<void()> f) {
    std::function<void(const Prop&, const Params&)> _parse = parse;
    parse = [_parse, f](const Prop& p, const Params& ps) {
      _parse(p, ps);
      f();
    };
  }
};
