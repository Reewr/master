#pragma once

#include <Substrate.h>
#include <string>
#include <vector>

#include "../Utils/str.hpp"

/**
 * @brief
 *   This class inherits from the NEAT:Substrate but does not extend
 *   any functionality on it besides loading/saving.
 *
 *   Since one can save the population and parameters but not the substrate,
 *   we felt it was nice to have that too.
 *
 *   The way this is meant to be used is to instansiate it like you would
 *   a normal NEAT::Substrate and use it like normal. The downcasting
 *   should happen automatically.
 */
class Substrate : public NEAT::Substrate {
public:
  typedef std::vector<std::vector<double>> Double2D;
  typedef std::vector<std::vector<int>>    Int2D;

  Substrate();
  Substrate(Double2D& inputs, Double2D& hidden, Double2D& ouputs);

  // Saves the substrate to a file of the given name
  void save(const std::string& filename);

  // Loads the substrate from a given file
  void load(const std::string& filename);

private:
  // Converts a generic value to string
  template <typename T>
  std::string saveValue(const std::string& name, T val);

  // Converts a 2D array of a generic value to string.
  // Currently only supports float, double and int
  template <typename T>
  std::string saveValue(const std::string& name, std::vector<std::vector<T>>&);

  // Loads a generic 2D array from a given string value,
  // Currently only supports float, double and int
  template <typename T>
  std::vector<std::vector<T>> loadArrayValue(const std::string& value);

  // Loads a generic value from string
  void loadValue(const std::string& value, bool& t);
  void loadValue(const std::string& value, int& t);
  void loadValue(const std::string& value, float& t);
  void loadValue(const std::string& value, double& t);
  void loadValue(const std::string& value, NEAT::ActivationFunction& t);
};

// Below follow template definitons of saveValue
// --------------------------------------------

template <typename T>
std::string Substrate::saveValue(const std::string& name, T val) {
  return name + " " + std::to_string(val);
}

/**
 * @brief
 *   This turns a generic 2D vector into a one line string.
 *
 *   The string will look something like:
 *
 *   [ [1, 2], [1, 2] ]
 *
 * @tparam T
 * @param name
 * @param d
 *
 * @return
 */
template <typename T>
std::string Substrate::saveValue(const std::string&           name,
                                 std::vector<std::vector<T>>& d) {
  std::string final = name + " [";

  for (auto& vec : d) {
    final += "[";

    for (unsigned int i = 0; i < vec.size(); i++) {
      if (i + 1 == vec.size())
        final += std::to_string(vec[i]);
      else
        final += std::to_string(vec[i]) + ",";
    }

    final += "]";
  }

  final += "]";

  return final;
}

/**
 * @brief
 *   This function handles loading array values from string by splitting
 *   the string on commas first and then parsing each of those elements as
 *   arrays.
 *
 * @tparam T
 * @param value
 *
 * @return
 */
template <typename T>
std::vector<std::vector<T>>
Substrate::loadArrayValue(const std::string& value) {
  std::string val         = value;
  size_t      firstSquare = val.find_first_of("[");
  size_t      lastSquare  = val.find_last_of("]");

  if (firstSquare == std::string::npos || lastSquare == std::string::npos) {
    throw std::runtime_error("Invalid format");
  }

  val = val.substr(firstSquare + 1, firstSquare + lastSquare - 1);

  if (val.find_first_of("1234567890") == std::string::npos) {
    return {};
  }

  std::vector<std::string> arrays;
  size_t                   startBrace = val.find_first_of("[");
  size_t                   endBrace   = val.find_first_of("]");
  while (startBrace != std::string::npos && endBrace != std::string::npos) {
    std::string item = val.substr(startBrace + 1, startBrace + endBrace - 1);

    if (item.size() != 0) {
      arrays.push_back(item);
    }

    val        = val.substr(startBrace + endBrace);
    startBrace = val.find_first_of("[");
    endBrace   = val.find_first_of("]");
  }

  std::vector<std::vector<T>> values;

  for (auto& s : arrays) {
    std::vector<T> numbers;
    for (auto& d : str::split(s, ',')) {
      if (std::is_same<int, T>())
        numbers.push_back(std::stoi(d));
      else
        numbers.push_back(std::stod(d));
    }

    values.push_back(numbers);
  }

  return values;
}
