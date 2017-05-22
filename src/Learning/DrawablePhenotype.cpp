#include "DrawablePhenotype.hpp"

#include <Genome.h>
#include <NeuralNetwork.h>

#include "../GLSL/Program.hpp"
#include "../Resource/ResourceManager.hpp"
#include "../Utils/Asset.hpp"
#include "../Utils/Utils.hpp"

#include "../3D/Line.hpp"
#include "../3D/Sphere.hpp"
#include "../Shape/GL/Shape.hpp"

#include "../GlobalLog.hpp"

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

DrawablePhenotype::DrawablePhenotype()
    : mNum3DLines(0)
    , mNumLines(0)
    , mNumFilledCircles(0)
    , mNumOutlineCircles(0)
    , mVBO(0)
    , mVAO(0)
    , mVBO3D(0)
    , mVAO3D(0) {
  ResourceManager* r = mAsset->rManager();

  mModelColorProgram = r->get<Program>("Program::Model");
  /* mLinesProgram         = r->get<Program>("Program::Lines"); */
  /* mOutlineCircleProgram = r->get<Program>("Program::CircleOutline"); */
  /* mFilledCircleProgram  = r->get<Program>("Program::CircleFilled"); */
}

DrawablePhenotype::~DrawablePhenotype() {
  glDeleteBuffers(1, &mVBO);
  glDeleteVertexArrays(1, &mVAO);
  glDeleteBuffers(1, &mVBO3D);
  glDeleteVertexArrays(1, &mVAO3D);
}

void DrawablePhenotype::input(const Input::Event&) {}
void DrawablePhenotype::update(float) {}

/**
 * @brief
 *   Finds the minimum and maximum substrate coordinates for the neural network.
 *
 *   Retrieves the min/max x, y. Will also try to retrieve the Z if that exists.
 *
 *   The two arguments, maxValues and minValues will be changed by this function
 *
 * @param network
 * @param maxValues
 * @param minValues
 */
void DrawablePhenotype::findSubstrateLimits(const NEAT::NeuralNetwork& network,
                                            vec3& maxValues,
                                            vec3& minValues) {

  maxValues = vec3(std::numeric_limits<float>::min());
  minValues = vec3(std::numeric_limits<float>::max());

  // Find maximum and minimum substrate coordinates
  for (auto& neuron : network.m_neurons) {
    if (neuron.m_substrate_coords[0] > maxValues.x)
      maxValues.x = neuron.m_substrate_coords[0];
    if (neuron.m_substrate_coords[0] < minValues.x)
      minValues.x = neuron.m_substrate_coords[0];

    if (neuron.m_substrate_coords[1] > maxValues.y)
      maxValues.y = neuron.m_substrate_coords[1];
    if (neuron.m_substrate_coords[1] < minValues.y)
      minValues.y = neuron.m_substrate_coords[1];

    if (neuron.m_substrate_coords.size() <= 2)
      continue;

    if (neuron.m_substrate_coords[2] > maxValues.z)
      maxValues.z = neuron.m_substrate_coords[2];
    if (neuron.m_substrate_coords[2] < minValues.z)
      minValues.z = neuron.m_substrate_coords[2];
  }
}

/**
 * @brief
 *   Goes through all the connections and checks their weight and returns
 *   the maximum weight of all connections.
 *
 * @param network
 *
 * @return
 */
float DrawablePhenotype::findMaxConnectionWeight(
  const NEAT::NeuralNetwork& network) {
  double maxWeight = 0;

  // Find the max weight of a connection
  if (network.m_connections.size()) {
    for (auto& c : network.m_connections) {
      float weight = mmm::abs(c.m_weight);

      if (weight > maxWeight)
        maxWeight = weight;
    }
  }

  return maxWeight;
}

/**
 * @brief
 *   Recreates the elements needed to draw the network in 3D
 *
 * @param network
 * @param size
 */
void DrawablePhenotype::recreate(const NEAT::NeuralNetwork& network,
                                 mmm::vec3                  size) {

  // double rectDiv          = size.x / 15.0;
  // double maxLineThickness = 3.0;
  double magn         = 255.0;
  double neuronRadius = size.x / 15;

  vec3 maxValues;
  vec3 minValues;

  findSubstrateLimits(network, maxValues, minValues);

  float maxWeight = findMaxConnectionWeight(network);

  if (mDrawables.size()) {
    for (auto& a : mDrawables)
      delete a;
    mDrawables.clear();
  }

  std::vector<GLShape::Vertex> lines;
  // Create a line for each connection, making the color depend on the
  // connections weight.
  //
  // weight < 0 and recur flag is true == green, else all colors
  // weight < 0 == blue, else red
  //
  // Lines go from one neuron to another
  //
  //  for (auto& conn : network.m_connections) {
  //    const NEAT::Neuron& source =
  //    network.m_neurons[conn.m_source_neuron_idx];
  //    const NEAT::Neuron& target =
  //    network.m_neurons[conn.m_target_neuron_idx];
  //    // double thickness =
  //    //   mmm::clamp(scale(conn.m_weight, 0, maxWeight, 1, maxLineThickness),
  //    //              1,
  //    //              maxLineThickness);
  //
  //    double w =
  //      mmm::clamp(scale(mmm::abs(conn.m_weight), 0.0, maxWeight, 0.0, 1.0),
  //                 0.2,
  //                 1.0);
  //
  //    vec4 color;
  //    vec3 start = vec3(source.m_substrate_coords[0],
  //                      source.m_substrate_coords[2],
  //                      source.m_substrate_coords[1]);
  //    vec3 end = vec3(target.m_substrate_coords[0],
  //                    target.m_substrate_coords[2],
  //                    target.m_substrate_coords[1]);
  //
  //    if (conn.m_recur_flag) {
  //      color = conn.m_weight < 0 ? vec4(0, w, 0, 1) : vec4(w);
  //    } else {
  //      color = conn.m_weight < 0 ? vec4(0, 0, w, 1) : vec4(w, 0, 0, 1);
  //    }
  //
  //    lines.push_back({ start, { 0, 0 }, color.xyz });
  //    lines.push_back({ end, { 0, 0 }, color.xyz });
  //
  //    color.w = 1;
  //  }
  //
  int inputHidden    = 0;
  int inputOutput    = 0;
  int hiddenOutput   = 0;
  int hiddenHidden   = 0;
  int outputHidden   = 0;
  int outputOutput   = 0;
  int loopedHidden   = 0;
  int loopedOutput   = 0;
  int biasHidden     = 0;
  int biasInput      = 0;
  int biasOutput     = 0;
  int numConnections = network.m_connections.size();
  int inputs         = network.m_num_inputs;
  int outputs        = network.m_num_outputs;
  int hiddens        = network.m_neurons.size() - inputs - outputs - 1;

  NEAT::NeuronType input  = NEAT::NeuronType::INPUT;
  NEAT::NeuronType output = NEAT::NeuronType::OUTPUT;
  NEAT::NeuronType bias   = NEAT::NeuronType::BIAS;
  NEAT::NeuronType hidden = NEAT::NeuronType::HIDDEN;

  for (auto& conn : network.m_connections) {
    const NEAT::Neuron& source = network.m_neurons[conn.m_source_neuron_idx];
    const NEAT::Neuron& target = network.m_neurons[conn.m_target_neuron_idx];

    if (source.m_type == input && target.m_type == hidden)
      inputHidden++;
    else if (source.m_type == input && target.m_type == output)
      inputOutput++;
    else if (source.m_type == hidden && target.m_type == output)
      hiddenOutput++;
    else if (source.m_type == hidden && target.m_type == hidden &&
             source == target)
      loopedHidden++;
    else if (source.m_type == hidden && target.m_type == hidden &&
             source == target)
      hiddenHidden++;
    else if (source.m_type == output && target.m_type == hidden)
      outputHidden++;
    else if (source.m_type == output && target.m_type == output &&
             source == target)
      loopedOutput++;
    else if (source.m_type == output && target.m_type == output)
      outputOutput++;
    else if (source.m_type == bias && target.m_type == output)
      biasOutput++;
    else if (source.m_type == bias && target.m_type == hidden)
      biasHidden++;
    else if (source.m_type == bias && target.m_type == input)
      biasInput++;

    // double thickness =
    //   mmm::clamp(scale(conn.m_weight, 0, maxWeight, 1, maxLineThickness),
    //              1,
    //              maxLineThickness);
    if (conn.m_weight == 0.0f)
      continue;

    double w =
      mmm::clamp(scale(mmm::abs(conn.m_weight), 0.0, maxWeight, 0.0, 1.0),
                 0.2,
                 1.0);

    vec4 color;
    vec3 start =
      vec3(source.m_substrate_coords[0],
           source.m_substrate_coords[1],
           source.m_substrate_coords.size() > 2 ? source.m_substrate_coords[2] :
                                                  0);
    vec3 end =
      vec3(target.m_substrate_coords[0],
           target.m_substrate_coords[1],
           target.m_substrate_coords.size() > 2 ? target.m_substrate_coords[2] :
                                                  0);

    if (conn.m_recur_flag) {
      color = conn.m_weight < 0 ? vec4(0, w, 0, 1) : vec4(w);
    } else {
      color = conn.m_weight < 0 ? vec4(0, 0, w, 1) : vec4(w, 0, 0, 1);
    }

    lines.push_back({ start, { 0, 0 }, color.xyz });
    lines.push_back({ end, { 0, 0 }, color.xyz });

    color.w = 1;
  }

  debug("Network has following:");
  debug("Total connections: {}", numConnections);
  debug("Input neurons: {}", inputs);
  debug("Output neurons: {}", outputs);
  debug("Hidden neurons: {}", hiddens);
  debug("Input to Hidden: {}", inputHidden);
  debug("Input to Output: {}", inputOutput);
  debug("Hidden to Output: {}", hiddenOutput);
  debug("Hidden to Hidden: {}", hiddenHidden);
  debug("Output to Hidden: {}", outputHidden);
  debug("Output to Output: {}", outputOutput);
  debug("Looped hidden: {}", loopedHidden);
  debug("Looped output: {}", loopedOutput);
  mNum3DLines = lines.size();

  Utils::assertGL();

  if (mVBO3D != 0)
    glDeleteBuffers(1, &mVBO3D);

  if (mVAO3D != 0)
    glDeleteVertexArrays(1, &mVAO3D);

  glGenBuffers(1, &mVBO3D);
  glGenVertexArrays(1, &mVAO3D);

  glBindVertexArray(mVAO3D);

  Utils::assertGL();
  glBindBuffer(GL_ARRAY_BUFFER, mVBO3D);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(GLShape::Vertex) * lines.size(),
               &lines[0],
               GL_STATIC_DRAW);
  Utils::assertGL();

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLShape::Vertex), 0);
  glVertexAttribPointer(1,
                        2,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(GLShape::Vertex),
                        (void*) (sizeof(mmm::vec3)));
  glVertexAttribPointer(2,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(GLShape::Vertex),
                        (void*) (sizeof(mmm::vec3) + sizeof(mmm::vec2)));
  Utils::assertGL();

  glBindVertexArray(0);
  Utils::assertGL();

  std::vector<Drawable3D*> filled;
  /* filled.resize(network.m_neurons.size()); */
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
    vec3 pos =
      vec3(neuron.m_substrate_coords[0],
           neuron.m_substrate_coords[1],
           neuron.m_substrate_coords.size() > 2 ? neuron.m_substrate_coords[2] :
                                                  0);

    float radiusFilled  = 0;
    float radiusOutline = 0;
    vec4  colorFilled;
    vec4  colorOutline;

    if (neuron.m_activation < 0) {
      colorFilled = vec4(0.3) + vec4(0, 0, 0.7, 1.0) * (-neuron.m_activation);
    } else {
      colorFilled = vec4(0.3) + vec4(0.7) * neuron.m_activation;
    }

    colorFilled.w = 1;

    if (neuron.m_type == NEAT::NeuronType::INPUT)
      colorOutline = vec4(0, 1, 0, 1);
    else if (neuron.m_type == NEAT::NeuronType::BIAS)
      colorOutline = vec4(0, 0, 0, 1);
    else if (neuron.m_type == NEAT::NeuronType::HIDDEN)
      colorOutline = vec4(0.5, 0.5, 0.5, 1);
    else if (neuron.m_type == NEAT::NeuronType::OUTPUT)
      colorOutline = vec4(1, 1, 0, 1);
    else if (neuron.m_type == NEAT::NeuronType::NONE)
      colorOutline = vec4(1, 0, 0, 1);

    radiusOutline = neuronRadius;
    radiusFilled = neuronRadius; // * mmm::clamp(neuron.m_activation, 0.3, 2.0);
    colorFilled  = mmm::clamp(colorFilled, 0.0, 1.0);

    filled.push_back(new Sphere(pos, radiusFilled, colorFilled));
    mDrawables.push_back(new Sphere(pos, radiusOutline, colorOutline, true));
  }

  mDrawables.insert(mDrawables.end(), filled.begin(), filled.end());
}

void DrawablePhenotype::recreate(const NEAT::NeuralNetwork& network,
                                 mmm::vec2                  size) {
  double rectDiv = size.x / 15.0;
  /* double maxLineThickness = 3.0; */
  double magn         = 255.0;
  double neuronRadius = 15;

  vec3 maxValues;
  vec3 minValues;

  findSubstrateLimits(network, maxValues, minValues);

  float maxWeight = findMaxConnectionWeight(network);

  // Use one vector to store all line and circle information so it
  // can be sent to OpenGL in one batch
  std::vector<Vertex> points(
    network.m_connections.size() + // The number of lines
    network.m_neurons.size() +     // The number of filled circles
    network.m_neurons.size()       // The number of outline circles
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
      line.color =
        conn.m_weight < 0 ? vec4(0, 0, magn * w, 1) : vec4(magn * w, 0, 0, 1);
    }

    line.color.w = 1;
    line.point =
      vec4(network.m_neurons[conn.m_source_neuron_idx].m_x,  // Start of P1
           network.m_neurons[conn.m_source_neuron_idx].m_y,  // Start of P1
           network.m_neurons[conn.m_target_neuron_idx].m_x,  // End of P1
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
                    minValues.x,
                    maxValues.x,
                    rectDiv,
                    size.x - rectDiv);
    float y = scale(neuron.m_substrate_coords[1],
                    minValues.y,
                    maxValues.y,
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
    filled.point.z  = neuronRadius * mmm::clamp(neuron.m_activation, 0.1, 1.0);
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

  mNumFilledCircles  = points.size() - mNumLines;
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
  glVertexAttribPointer(1,
                        4,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(Vertex),
                        (void*) (sizeof(mmm::vec4)));

  glBindVertexArray(0);

  points.clear();
  outlineCircles.clear();
}

/**
 * @brief
 *   Draw the lines, filled circles and outline circles that represents the
 * network
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
  glDrawArrays(GL_POINTS,
               mNumLines + mNumFilledCircles,
               mNumOutlineCircles - mNumLines);

  glBindVertexArray(0);
}

void DrawablePhenotype::draw3D(mmm::vec3 offset) {
  if (mVAO3D == 0)
    return;

  for (auto& d : mDrawables) {
    if (d == nullptr)
      std::cout << "nullptr" << std::endl;
    else
      d->draw(mModelColorProgram, offset, true);
  }

  mModelColorProgram->bind();
  mModelColorProgram->setUniform("model", mmm::translate(offset));
  mModelColorProgram->setUniform("useNormalsAsColors", true);

  glBindVertexArray(mVAO3D);

  glDrawArrays(GL_LINES, 0, mNum3DLines);

  glBindVertexArray(0);

  mModelColorProgram->setUniform("useNormalsAsColors", false);
}

void DrawablePhenotype::save(const std::string&) {}
