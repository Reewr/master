#include "Substrate.hpp"

#include <mmm.hpp>
#include <fstream>
#include <map>

#include "../Utils/str.hpp"

Substrate::Substrate() : NEAT::Substrate() {}

Substrate::Substrate(Substrate::Double2D& inputs,
                     Substrate::Double2D& hidden,
                     Substrate::Double2D& ouputs)
    : NEAT::Substrate(inputs, hidden, ouputs) {}

/**
 * @brief
 *   Saves the substrate to a file with the given filename.
 *
 *   All the values of the Substrate is saved to file.
 *
 * @param filename
 */
void Substrate::save(const std::string& filename) {
  std::ofstream fs(filename);

  if (!fs.is_open())
    throw std::runtime_error("Unable to open substrate file: " + filename);

  fs << saveValue("m_input_coords", m_input_coords) << std::endl;
  fs << saveValue("m_hidden_coords", m_hidden_coords) << std::endl;
  fs << saveValue("m_output_coords", m_output_coords) << std::endl;
  fs << saveValue("m_leaky", m_leaky) << std::endl;
  fs << saveValue("m_with_distance", m_with_distance) << std::endl;
  fs << saveValue("m_allow_input_hidden_links", m_allow_input_hidden_links)
     << std::endl;
  fs << saveValue("m_allow_input_output_links", m_allow_input_output_links)
     << std::endl;
  fs << saveValue("m_allow_hidden_hidden_links", m_allow_hidden_hidden_links)
     << std::endl;
  fs << saveValue("m_allow_hidden_output_links", m_allow_hidden_output_links)
     << std::endl;
  fs << saveValue("m_allow_output_hidden_links", m_allow_output_hidden_links)
     << std::endl;
  fs << saveValue("m_allow_output_output_links", m_allow_output_output_links)
     << std::endl;
  fs << saveValue("m_allow_looped_hidden_links", m_allow_looped_hidden_links)
     << std::endl;
  fs << saveValue("m_allow_looped_output_links", m_allow_looped_output_links)
     << std::endl;
  fs << saveValue("m_custom_connectivity", m_custom_connectivity) << std::endl;
  fs << saveValue("m_custom_conn_obeys_flags", m_custom_conn_obeys_flags)
     << std::endl;
  fs << saveValue("m_query_weights_only", m_query_weights_only) << std::endl;
  fs << saveValue("m_hidden_nodes_activation", m_hidden_nodes_activation)
     << std::endl;
  fs << saveValue("m_output_nodes_activation", m_output_nodes_activation)
     << std::endl;
  fs << saveValue("m_max_weight_and_bias", m_max_weight_and_bias) << std::endl;
  fs << saveValue("m_min_time_const", m_min_time_const) << std::endl;
  fs << saveValue("m_max_time_const", m_max_time_const) << std::endl;

  fs.close();
}

/**
 * @brief
 *   Loads the substrate from a given file, will load everything it
 *   can falling back on default values for those that are not specified.
 *
 *   Will throw error if any parsing / loading error happens.
 *
 * @param filename
 */
void Substrate::load(const std::string& filename) {
  std::ifstream fs(filename);

  if (!fs.is_open())
    throw std::runtime_error("Unable to open substrate file: " + filename);

  std::map<std::string, std::string> entries;
  std::string line;
  size_t lineNum = 0;

  // Expect each line to contain the name and the value separated by space
  while(fs.good()) {
    std::getline(fs, line);
    size_t spacePos = line.find_first_of(" ");

    if (spacePos == std::string::npos)
      throw std::runtime_error("Invalid line:" + std::to_string(lineNum));

    std::string name = line.substr(0, spacePos);
    std::string val  = line.substr(spacePos);

    entries[name] = val;

    lineNum++;
  }

  for (auto& a : entries) {
    if (a.first == "m_input_coords")
      m_input_coords = loadArrayValue<double>(a.second);
    else if (a.first == "m_hidden_coords")
      m_hidden_coords = loadArrayValue<double>(a.second);
    else if (a.first == "m_output_coords")
      m_output_coords = loadArrayValue<double>(a.second);
    else if (a.first == "m_leaky")
      m_leaky = loadValue<bool>(a.second);
    else if (a.first == "m_with_distance")
      m_with_distance = loadValue<bool>(a.second);
    else if (a.first == "m_allow_input_hidden_links")
      m_allow_input_hidden_links = loadValue<bool>(a.second);
    else if (a.first == "m_allow_input_output_links")
      m_allow_input_output_links = loadValue<bool>(a.second);
    else if (a.first == "m_allow_hidden_hidden_links")
      m_allow_hidden_hidden_links = loadValue<bool>(a.second);
    else if (a.first == "m_allow_hidden_output_links")
      m_allow_hidden_output_links = loadValue<bool>(a.second);
    else if (a.first == "m_allow_output_hidden_links")
      m_allow_output_hidden_links = loadValue<bool>(a.second);
    else if (a.first == "m_allow_output_output_links")
      m_allow_output_output_links = loadValue<bool>(a.second);
    else if (a.first == "m_allow_looped_hidden_links")
      m_allow_looped_hidden_links = loadValue<bool>(a.second);
    else if (a.first == "m_allow_looped_output_links")
      m_allow_looped_output_links = loadValue<bool>(a.second);
    else if (a.first == "m_custom_connectivity")
      m_custom_connectivity = loadArrayValue<int>(a.second);
    else if (a.first == "m_custom_conn_obeys_flags")
      m_custom_conn_obeys_flags = loadValue<bool>(a.second);
    else if (a.first == "m_query_weights_only")
      m_query_weights_only = loadValue<bool>(a.second);
    else if (a.first == "m_hidden_nodes_activation")
      m_hidden_nodes_activation = loadValue<NEAT::ActivationFunction>(a.second);
    else if (a.first == "m_output_nodes_activation")
      m_output_nodes_activation = loadValue<NEAT::ActivationFunction>(a.second);
    else if (a.first == "m_max_weight_and_bias")
      m_max_weight_and_bias = loadValue<double>(a.second);
    else if (a.first == "m_min_time_const")
      m_min_time_const = loadValue<double>(a.second);
    else if (a.first == "m_max_time_const")
      m_max_time_const = loadValue<double>(a.second);
  }

  // Load each and every variable if they exist in the file

  fs.close();
}

/**
 * @brief
 *   Pads the name to a specific length.
 *
 * @param name
 *
 * @return
 */
std::string Substrate::fillName(const std::string& name) {
  // Describes the maximum length of the
  // variable names in NEAT::Substrate
  static int nameLength = 30;
  int fillLength = mmm::max(0, nameLength - name.size());

  return name + std::string(" ", fillLength);
}
