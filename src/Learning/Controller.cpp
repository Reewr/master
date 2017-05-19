#include "Controller.hpp"

#include "Phenotype.hpp"
#include "Substrate.hpp"
#include "../Experiments/Experiment.hpp"
#include "../Input/Event.hpp"
#include "../OpenGLHeaders.hpp"

#include <NeuralNetwork.h>

void loadExperiments(Experiment* experiment, const std::string& filename) {
  std::string population = filename + ".population";
  std::string substrate  = filename + ".substrate";

  experiment->setPopulation(new NEAT::Population(population.c_str()));
  experiment->substrate()->load(substrate);
}

void buildNetwork(Experiment*          experiment,
                  NEAT::NeuralNetwork* network,
                  NEAT::Genome&        g) {
  if (experiment->parameters().useESHyperNEAT) {
    g.BuildESHyperNEATPhenotype(*network,
                                *experiment->substrate(),
                                experiment->population()->m_Parameters);
  } else {
    g.BuildHyperNEATPhenotype(*network, *experiment->substrate());
  }
}

Controller::Controller(Experiment*        standing,
                       const std::string& standingExperiment,
                       Experiment*        walking,
                       const std::string& walkingExperiment) {
  mStanding = standing;
  mWalking  = walking;

  loadExperiments(mStanding, standingExperiment);
  loadExperiments(mWalking, walkingExperiment);

  std::string genomeFilename = standingExperiment + ".genome";

  std::ifstream standFS;
  standFS.open(genomeFilename);

  if (standFS.is_open())
    mBestStander = NEAT::Genome(genomeFilename.c_str());
  else
    mBestStander = mStanding->population()->GetBestGenome();

  standFS.close();

  genomeFilename = walkingExperiment + ".genome";
  std::ifstream walkFS;
  walkFS.open(genomeFilename);

  if (walkFS.is_open())
    mBestWalker = NEAT::Genome(genomeFilename.c_str());
  else
    mBestWalker = mWalking->population()->GetBestGenome();

  walkFS.close();

  mPhenotype = new Phenotype();
  mStandingNetwork = new NEAT::NeuralNetwork();
  mWalkingNetwork = new NEAT::NeuralNetwork();

  mPhenotype->reset(0, 0, 0, 0);
  mTempNetwork = mPhenotype->network;
}

Controller::~Controller() {
  mPhenotype->network = mTempNetwork;
  mPhenotype->remove();

  delete mPhenotype;
  delete mStandingNetwork;
  delete mWalkingNetwork;
}

void Controller::changeStage(Controller::Stage stage) {
  if (mCurrentStage == Stage::None && stage != Stage::None) {

    mStandingNetwork->Flush();
    mStandingNetwork->Clear();
    mWalkingNetwork->Flush();
    mWalkingNetwork->Clear();

    buildNetwork(mStanding, mStandingNetwork, mBestStander);
    buildNetwork(mWalking, mWalkingNetwork, mBestWalker);
  } else if (mCurrentStage != Stage::None && stage == Stage::None) {
    mPhenotype->reset(0, 0, 0, 0);
  }

  if (stage == Stage::Walking && mCurrentStage != Stage::Walking) {
    mPhenotype->network = mWalkingNetwork;
  } else if (stage == Stage::Standing && mCurrentStage != Stage::Standing) {
    mPhenotype->network = mStandingNetwork;
  }

  mCurrentStage = stage;
}

/**
 * @brief
 *   Handles the input for hte controller where certain
 *   keys change the state
 *
 * @param event
 */
void Controller::input(const Input::Event& event) {
  // Supporting dvorak too, so on dvorak, its the JKL keys on a querty layout
  if (event.keyPressed(GLFW_KEY_I) || event.keyPressed(GLFW_KEY_H)) {
    changeStage(Stage::None);
    return event.stopPropgation();
  }

  if (event.keyPressed(GLFW_KEY_O) || event.keyPressed(GLFW_KEY_T)) {
    changeStage(Stage::Standing);
    return event.stopPropgation();
  }

  if (event.keyPressed(GLFW_KEY_P) || event.keyPressed(GLFW_KEY_N)) {
    changeStage(Stage::Walking);
    return event.stopPropgation();
  }
}

void Controller::update(float) {
  // The experiment isnt actually used since it is the warmup period
  if (mPhenotype->duration < 0)
    return mPhenotype->update(*mWalking);

  if (mCurrentStage == Stage::Walking) {
    mPhenotype->update(*mWalking);
  } else if (mCurrentStage == Stage::Standing) {
    mPhenotype->update(*mStanding);
  }

  mPhenotype->failed = false;
}

void Controller::draw(std::shared_ptr<Program>& prog, bool bindTexture) {
  mPhenotype->draw(prog, mmm::vec3(0), bindTexture);
}
