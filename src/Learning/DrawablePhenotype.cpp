#include "DrawablePhenotype.hpp"

#include <Genome.h>
#include <NeuralNetwork.h>

#include "../Utils/Asset.hpp"
#include "../Resource/ResourceManager.hpp"
#include "../GLSL/Program.hpp"

using mmm::vec2;
using mmm::vec3;
using mmm::vec4;

float scale(double coord,
            double minCoord,
            double maxCoord,
            double domainMin,
            double domainMax) {
  double diff = maxCoord - minCoord;

  if (diff == 0)
    return maxCoord;

  double tr   = domainMax - domainMin;
  double relA = (coord - minCoord) / diff;

  return domainMin + tr * relA;
}

DrawablePhenotype::DrawablePhenotype() {
  ResourceManager* r = mAsset->rManager();

  mLinesProgram         = r->get<Program>("Program::Lines");
  mOutlineCircleProgram = r->get<Program>("Program::CircleOutline");
  mFilledCircleProgram  = r->get<Program>("Program::CircleFilled");
}

DrawablePhenotype::~DrawablePhenotype() {
  glDeleteBuffers(1, &mVBO);
  glDeleteVertexArrays(1, &mVAO);
}

void DrawablePhenotype::input(const Input::Event&) { }
void DrawablePhenotype::update(float) { }

void DrawablePhenotype::recreate(const NEAT::NeuralNetwork& network,
                                 mmm::vec2 size) {
  double rectDiv          = size.x / 15.0;
  double magn             = 255.0;
  double maxLineThickness = 3.0;
  double neuronRadius     = 15;

  double maxX = std::numeric_limits<double>::min();
  double maxY = std::numeric_limits<double>::min();
  double minX = std::numeric_limits<double>::max();
  double minY = std::numeric_limits<double>::max();

  // Find maximum and minimum substrate coordinates
  for (auto& neuron : network.m_neurons) {
    if (neuron.m_substrate_coords[0] > maxX)
      maxX = neuron.m_substrate_coords[0];
    if (neuron.m_substrate_coords[0] < minX)
      minX = neuron.m_substrate_coords[0];

    if (neuron.m_substrate_coords[1] > maxY)
      maxY = neuron.m_substrate_coords[1];
    if (neuron.m_substrate_coords[1] < minY)
      minY = neuron.m_substrate_coords[1];
  }

  double maxWeight = 0;

  // Find the max weight of a connection
  if (network.m_connections.size()) {
    for (auto& c : network.m_connections) {
      float weight = mmm::abs(c.m_weight);

      if (weight > maxWeight)
        maxWeight = weight;
    }
  }

  // Use one vector to store all line and circle information so it
  // can be sent to OpenGL in one batch
  std::vector<Vertex> points(
      network.m_connections.size() + // The number of lines
      network.m_neurons.size() + // The number of filled circles
      network.m_neurons.size()   // The number of outline circles
  );

  // Create a line for each connection, making the color depend on the
  // connections weight.
  //
  // weight < 0 and recur flag is true == green, else all colors
  // weight < 0 == blue, else red
  //
  // Lines go from one neuron to another
  for (auto& conn : network.m_connections) {
    // double thickness =
    //   mmm::clamp(scale(conn.m_weight, 0, maxWeight, 1, maxLineThickness),
    //              1,
    //              maxLineThickness);

    double w =
      mmm::clamp(scale(mmm::abs(conn.m_weight), 0.0, maxWeight, 0.0, 1.0),
                 0.75,
                 1.0);

    Vertex line;

    if (conn.m_recur_flag) {
      line.color = conn.m_weight < 0 ? vec4(0, magn * w, 0, 1) : vec4(magn * w);
    } else {
      line.color = conn.m_weight < 0 ? vec4(0, 0, magn * w, 1) : vec4(magn * w, 0, 0, 1);
    }

    line.color.w  = 1;
    line.point = vec4(network.m_neurons[conn.m_source_neuron_idx].m_x, // Start of P1
                      network.m_neurons[conn.m_source_neuron_idx].m_y, // Start of P1
                      network.m_neurons[conn.m_target_neuron_idx].m_x, // End of P1
                      network.m_neurons[conn.m_target_neuron_idx].m_y); // End of P1

    points.push_back(line);
  }

  mNumLines = points.size();

  // Use a seperate vector for outlineCircles so that we can add all
  // of the filled circles to the end of the points vector, keeping them
  // sorted.
  std::vector<Vertex> outlineCircles(network.m_neurons.size());

  // For each neuron where an outline of the circle indicates the type
  // of neuron.
  //
  // Input = green
  // Bias  = black
  // Hidden = gray
  // output = yellow
  // NONE = red
  //
  // The size of the filled circle and its color is dependent on the activation
  // of that neuron.
  for (auto& neuron : network.m_neurons) {
    Vertex filled;
    Vertex outline;

    float x = scale(neuron.m_substrate_coords[0],
                       minX,
                       maxX,
                       rectDiv,
                       size.x - rectDiv);
    float y = scale(neuron.m_substrate_coords[1],
                       minY,
                       maxY,
                       rectDiv,
                       size.y - rectDiv);

    filled.point  = vec4(x, y, 0.0, 0.0);
    outline.point = vec4(x, y, 0.0, 0.0);


    if (neuron.m_activation < 0) {
      filled.color = vec4(0.3) + vec4(0, 0, 0.7, 1.0) * (-neuron.m_activation);
    } else {
      filled.color = vec4(0.3) + vec4(0.7) * neuron.m_activation;
    }

    filled.color.w = 1;

    if (neuron.m_type == NEAT::NeuronType::INPUT)
      outline.color = vec4(0, 1, 0, 1);
    else if (neuron.m_type == NEAT::NeuronType::BIAS)
      outline.color = vec4(0, 0, 0, 1);
    else if (neuron.m_type == NEAT::NeuronType::HIDDEN)
      outline.color = vec4(0.5, 0.5, 0.5, 1);
    else if (neuron.m_type == NEAT::NeuronType::OUTPUT)
      outline.color = vec4(1, 1, 0, 1);
    else if (neuron.m_type == NEAT::NeuronType::NONE)
      outline.color = vec4(1, 0, 0, 1);

    outline.point.z = neuronRadius;
    filled.point.z  = neuronRadius * mmm::clamp(neuron.m_activation, 0.3, 2.0);
    filled.color    = mmm::clamp(filled.color, 0.0, 1.0);

    points.push_back(filled);
    outlineCircles.push_back(outline);
  }

  // Combine all the lines/circles into one list where the lines are first,
  // followed by filled circles and lastly outlined circles
  //
  // This is more effective than keeping three seperate vectors due to less
  // copying/moving
  points.insert(points.end(), outlineCircles.begin(), outlineCircles.end());

  mNumFilledCircles = points.size() - mNumLines;
  mNumOutlineCircles = outlineCircles.size();

  // Generate VBO and send the point data to OpenGL as one huge block, which
  // is more efficient than seperate.
  //
  // The added bonus of this is that we use one VertexArray and dont need to
  // bind between several when drawing, even though we have to use three
  // seperate with three seperate draw calls.
  //
  if (mVBO == 0)
    glGenBuffers(1, &mVBO);

  if (mVAO == 0)
    glGenVertexArrays(1, &mVAO);

  glBindVertexArray(mVAO);

  glBindBuffer(GL_ARRAY_BUFFER, mVBO);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(Vertex) * points.size(),
               &points[0],
               GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) (sizeof(mmm::vec4)));

  glBindVertexArray(0);

  points.clear();
  outlineCircles.clear();
}

/**
 * @brief
 *   Draw the lines, filled circles and outline circles that represents the network
 */
void DrawablePhenotype::draw() {
  // If the element isnt setup, dont do anything
  if (mVAO == 0)
    return;

  glBindVertexArray(mVAO);

  // Draw lines first
  mLinesProgram->bind();
  glDrawArrays(GL_POINTS, 0, mNumLines);

  // Draw filled circles
  mFilledCircleProgram->bind();
  glDrawArrays(GL_POINTS, mNumLines, mNumFilledCircles - mNumLines);

  // Draw outline circles
  mOutlineCircleProgram->bind();
  glDrawArrays(GL_POINTS, mNumLines + mNumFilledCircles, mNumOutlineCircles - mNumLines);

  glBindVertexArray(0);
}

void DrawablePhenotype::save(const std::string& filename) {}
