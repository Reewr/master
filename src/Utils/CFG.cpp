#include "CFG.hpp"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <sstream>

#include "../GlobalLog.hpp"
#include "../Input/Input.hpp"
#include "Utils.hpp"
#include "str.hpp"

using mmm::vec2;

ActB::ActB(int k1, int k2) {
  key1 = k1;
  key2 = k2;
}

CFG::CFG() : Logging::Log("CFG") {
  this->special_cases();
}

void CFG::unknown_parameter(const Prop&        p,
                            const Param&       pm,
                            const std::string& v) {
  error("Failed to parse '{}': unknown parameter: '{}'. Possible: {}",
        p,
        pm,
        v);
}

CFG::Wrapper::Wrapper(bool& b) {
  valid_params = " on off";

  parse = [&b](const Prop& p, const Params& ps) {
    if (ps[0] == "off")
      b = false;
    else if (ps[0] == "on")
      b = true;
    else
      unknown_parameter(p, ps[0], " on off");
  };

  show   = [&b]() { return b ? "on" : "off"; };
  asType = [&b]() { return b ? "true" : "false"; };
}
CFG::Wrapper::Wrapper(int& i) {
  valid_params = " number";

  parse = [&i](const Prop& p, const Params& ps) {

    std::stringstream ss(ps[0]);
    ss >> i;

    if (ss.fail())
      unknown_parameter(p, ps[0], " number");
  };

  show   = [&i]() { return std::to_string(i); };
  asType = [&i]() { return std::to_string(i); };
}
CFG::Wrapper::Wrapper(float& f) {
  valid_params = " number";

  parse = [&f](const Prop& p, const Params& ps) {

    std::stringstream ss(ps[0]);
    ss >> f;

    if (ss.fail())
      unknown_parameter(p, ps[0], " number");
  };

  show   = [&f]() { return std::to_string(f); };
  asType = [&f]() { return std::to_string(f); };
}
CFG::Wrapper::Wrapper(vec2& v) : args(2) {
  valid_params = " number number";

  parse = [&v](const Prop& p, const Params& ps) {
    std::stringstream s1(ps[0]);
    std::stringstream s2(ps[1]);
    s1 >> v.x;
    s2 >> v.y;

    if (s1.fail())
      unknown_parameter(p, ps[0], " number");

    else if (s2.fail())
      unknown_parameter(p, ps[1], " number");
  };

  show = [&v]() {
    std::string s;
    s += std::to_string(v.x);
    s += " ";
    s += std::to_string(v.y);
    return s;
  };

  asType = [&v]() {
    return "vec2(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ")";
  };
}

CFG::Wrapper::Wrapper(ActB& ab) : args(2) {
  parse = [&ab](const Prop& p, const Params& ps) {
    try {
      ab.key1 = Input::keyMap.at(ps[0]);
    } catch (...) {
      std::string valid_parameters = "";
      for (const auto& kv : Input::keyMap)
        valid_parameters += " " + kv.first;
      unknown_parameter(p, ps[0], valid_parameters);
    }

    try {
      ab.key2 = Input::keyMap.at(ps[1]);
    } catch (...) {
      std::string valid_parameters = "";
      for (const auto& kv : Input::keyMap)
        valid_parameters += " " + kv.first;
      unknown_parameter(p, ps[1], valid_parameters);
    }
  };

  show = [&ab]() {
    std::string keys;
    for (const auto& s : Input::keyMap)
      if (ab.key1 == s.second)
        keys += s.first + " ";

    for (const auto& s : Input::keyMap)
      if (ab.key2 == s.second)
        keys += s.first;
    return keys;
  };

  asType = [&]() { return show(); };
}

void CFG::setProp(const Prop& p, const Params& ps) {
  if (map.find(p) == map.end()) {
    mLog->warn("Unknown setting '{}'", p);
    return;
  }

  unsigned int args = map.at(p).args;

  if (args > ps.size()) {
    mLog->warn("Failed to parse '{}'. Missing parameter, expected: {}",
               p,
               args);
    return;
  }

  else if (args < ps.size())
    mLog->warn("Failed to parse '{}'. Excessive parameter, expected: {}",
               p,
               args);

  map.at(p).parse(p, ps);
}

std::string CFG::getProp(const Prop& p) {
  return map.at(p).show();
}

/**
 * @brief
 *   Returns the property as what is needed to normally construct it.
 *   For instance, Graphics.Resolution will return `vec2(x, y)` where
 *   x and y are float values.
 *
 * @param p the property to find, ex: `Graphics.Resolution`.
 *
 * @return the constructing type, or empty string if the property is not found
 */
std::string CFG::getPropAsType(const Prop& p) {
  if (map.count(p) == 0)
    return "";
  return map.at(p).asType();
}

int CFG::get_num_params(const Prop& p) {
  if (map.find(p) != map.end())
    return map.at(p).args;

  return -1;
}

void CFG::assimilate(const std::string& filepath) {

  mLog->debug("Assimilating '{}'", filepath);

  // spaghetti mode enabled!

  std::ifstream file(filepath);
  if (!file.is_open())
    return;

  std::string section = "";
  std::string line;

  while (std::getline(file, line)) {
    str::lTrim(line);

    // comment
    if (line.substr(0, 1) == "#")
      continue;

    // new section
    if (line.substr(0, 1) == "[") {
      size_t f = line.find("]");

      if (f != std::string::npos)
        section = line.substr(1, f - 1);
    }

    // setting
    std::string key;
    size_t      s = line.find_first_of(" \t");

    if (s == std::string::npos)
      continue;
    key  = line.substr(0, s);
    line = line.substr(s);

    // define the parameter array
    Params params;

    // while more values
    while (true) {
      std::string val;
      str::lTrim(line);

      if (line.substr(0, 1) == "#")
        break;

      s = line.find_first_of(" \t");
      if (s == std::string::npos && line.size() == 0)
        break;

      if (s == std::string::npos) {
        val = line.substr(0);
        params.push_back(val);
        break;
      } else {
        val = line.substr(0, s);
        params.push_back(val);
        line = line.substr(s);
      }
    }

    setProp(section + "." + key, params);
  }

  file.close();
}

void CFG::assimilate(int argc, char* argv[]) {
  if (argc > 1)
    mLog->debug("Assimilating launch parameters");

  int i = 1;

  while (i < argc) {
    Params params;

    int n  = get_num_params(argv[i]);
    int n2 = (n > 0) ? n : 0;

    while (n > 0) {
      params.push_back(argv[i + n]);
      n -= 1;
    }

    std::reverse(params.begin(), params.end());
    setProp(argv[i], params);

    i += 1 + n2;
  }
}


bool CFG::writetoFile(std::string filename) {
  std::ofstream file(filename);
  if (file.is_open()) {
    file << *this;
    mLog->debug("Writing to '{}'", filename);
  } else {
    file.open(filename,
              std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
    mLog->debug("Creating '{}'", filename);
    if (file.is_open()) {
      file << *this;
      mLog->debug("Writing to '{}'", filename);
    } else {
      mLog->error("Could not create file: '{}'", filename);
      return false;
    }
  }
  file.close();
  return true;
}


typedef std::map<std::string, std::map<std::string, std::string>> StrCFG;

std::ostream& operator<<(std::ostream& os, const CFG& cfg) {
  StrCFG strcfg;

  // add all properties to strcfg
  size_t pos;
  for (const auto& p : cfg.map) {
    pos = p.first.find(".");
    strcfg[p.first.substr(0, pos)][p.first.substr(pos + 1)] = p.second.show();
  }

  // remove duplicate entries
  for (const auto& prop : cfg.duplicates) {
    pos = prop.find(".");

    auto section = strcfg.find(prop.substr(0, pos));
    if (section != strcfg.end()) {
      auto key = section->second.find(prop.substr(pos + 1));

      if (key != section->second.end())
        section->second.erase(key);
    }
  }

  // print to stream
  os << "# Config file generated by Woooooo" << std::endl << std::endl;

  for (const auto& section : strcfg) {
    os << "[" << section.first << "]" << std::endl;

    if (section.first == "Bindings") {
      os << "  # List of possible keybindings on a US Keyboard:" << std::endl;
      os << "  #";
      int i = 0;
      for (const auto& kv : Input::keyMap) {
        if (i > 50) {
          os << std::endl << "  #";
          i = 0;
        }
        os << " " + kv.first;
        i += kv.first.size();
      }
      os << std::endl << std::endl;
    }

    for (const auto& key : section.second) {
      if (section.first != "Bindings") {
        os << std::endl << "  # possible values are:";
        os << cfg.map.at(section.first + "." + key.first).valid_params;
        os << std::endl;
      }

      std::string s = key.first;
      s.resize(22, ' ');
      os << "  " << s << key.second << std::endl;
    }
    os << std::endl;
  }

  // return the stream
  return os;
}
