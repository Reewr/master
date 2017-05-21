#pragma once

#include <string>
#include <memory>

#include <Genome.h>

class Experiment;
class Phenotype;
class Program;

namespace Input {
  class Event;
}

namespace NEAT {
  class NeuralNetwork;
}

class Controller {
public:
  enum class Stage {
    None,
    Standing,
    Walking
  };

  Controller(Experiment* standing,
             const std::string& standingExperiment,
             Experiment* walking,
             const std::string& walkingExperiment);

  ~Controller();

  void input(const Input::Event& event);

  void draw(std::shared_ptr<Program>& prog, bool bindTexture);

  void update(float deltaTime);

  const Phenotype& phenotype() const;

  Stage stage() const;

private:
  void changeStage(Stage stage);

  NEAT::NeuralNetwork* mStandingNetwork;
  NEAT::NeuralNetwork* mWalkingNetwork;
  NEAT::NeuralNetwork* mTempNetwork;

  NEAT::Genome mBestWalker;
  NEAT::Genome mBestStander;

  Phenotype* mPhenotype;

  Experiment* mStanding;
  Experiment* mWalking;

  Stage mCurrentStage;
};
