#include "WalkingRotationInputs.hpp"

#include "ExperimentUtil.hpp"

#include "../Learning/Phenotype.hpp"
#include "../Learning/Substrate.hpp"

#include <btBulletDynamicsCommon.h>

const float PI = mmm::constants<float>::pi;

WalkingRotationInputs::WalkingRotationInputs() : Experiment("WalkingRotationInputs") {

  mParameters.numActivates = 6;
  mFitnessFunctions =
  { Fitness("Movement",
            "Fitness based on movement in positive z direction.",
            [](const Phenotype& p, float, float) -> float {
              const auto&        parts = p.spider->parts();
              const btRigidBody* sternum =
                parts.at("Sternum").part->rigidBody();
              const btVector3& massPos = sternum->getCenterOfMassPosition();

              return massPos.z();
            },
            [](const Phenotype&, float current, float) -> float {
              return mmm::max(current, 0);
            }),
    Fitness("Colliding",
            "If the spider falls, stop the simulation",
            [](const Phenotype& phenotype, float current, float) -> float {

              if (phenotype.collidesWithTerrain("Abdomin") ||
                  phenotype.collidesWithTerrain("Sternum") ||
                  phenotype.collidesWithTerrain("Eye") ||
                  phenotype.collidesWithTerrain("Hip") ||
                  phenotype.collidesWithTerrain("Neck") ||
                  phenotype.collidesWithTerrain("PatellaR1") ||
                  phenotype.collidesWithTerrain("PatellaR2") ||
                  phenotype.collidesWithTerrain("PatellaR3") ||
                  phenotype.collidesWithTerrain("PatellaR4") ||
                  phenotype.collidesWithTerrain("PatellaL1") ||
                  phenotype.collidesWithTerrain("PatellaL2") ||
                  phenotype.collidesWithTerrain("PatellaL3") ||
                  phenotype.collidesWithTerrain("PatellaL4") ||
                  phenotype.collidesWithTerrain("FemurR1") ||
                  phenotype.collidesWithTerrain("FemurR2") ||
                  phenotype.collidesWithTerrain("FemurR3") ||
                  phenotype.collidesWithTerrain("FemurR4") ||
                  phenotype.collidesWithTerrain("FemurL1") ||
                  phenotype.collidesWithTerrain("FemurL2") ||
                  phenotype.collidesWithTerrain("FemurL3") ||
                  phenotype.collidesWithTerrain("FemurL4") ||
                  phenotype.collidesWithTerrain("TrochanterR1") ||
                  phenotype.collidesWithTerrain("TrochanterR2") ||
                  phenotype.collidesWithTerrain("TrochanterR3") ||
                  phenotype.collidesWithTerrain("TrochanterR4") ||
                  phenotype.collidesWithTerrain("TrochanterL1") ||
                  phenotype.collidesWithTerrain("TrochanterL2") ||
                  phenotype.collidesWithTerrain("TrochanterL3") ||
                  phenotype.collidesWithTerrain("TrochanterL4")) {
                phenotype.kill();
              }

              return current;
            }),
  };

  // When a new connection, it will not be added if the weight*maxWeightAndBias
  // is less than 0.2
  mSubstrate = createDefaultSubstrate();
  mSubstrate->m_hidden_nodes_activation = NEAT::ActivationFunction::TANH;
  mSubstrate->m_output_nodes_activation = NEAT::ActivationFunction::TANH;
  mSubstrate->m_max_weight_and_bias = 4.0;

  NEAT::Parameters params = getDefaultParameters();
  params.SurvivalRate = 0.25;
  params.MultipointCrossoverRate = 0.75;
  params.EliteFraction = 0.2;
  params.MutateAddNeuronProb = 0.03;
  params.MutateAddLinkProb = 0.2;
  params.MaxWeight = 4.0;
  params.CompatTreshold = 2.0;

  NEAT::Genome genome(0,
                      mSubstrate->GetMinCPPNInputs(),
                      0,
                      mSubstrate->GetMinCPPNOutputs(),
                      false,
                      NEAT::ActivationFunction::SIGNED_SINE,
                      NEAT::ActivationFunction::SIGNED_GAUSS,
                      0,
                      params);

  mPopulation = new NEAT::Population(genome, params, true, params.MaxWeight, time(0));
  mParameters.numActivates = 8;
}

WalkingRotationInputs::~WalkingRotationInputs() {
  delete mPopulation;
  delete mSubstrate;
}

void WalkingRotationInputs::outputs(Phenotype&                 p,
                                  const std::vector<double>& outputs) const {
  size_t index = 16;
  for(auto& part : p.spider->parts()) {
    if (part.second.hinge == nullptr)
      continue;

    float currentAngle = part.second.hinge->getHingeAngle();
    float velocity;

    if (part.second.active) {
      float output = outputs[index];

      currentAngle = ExpUtil::normalizeAngle(currentAngle, -PI, PI, 0);
      velocity     = output - currentAngle;
      index++;
    } else {
      velocity = mmm::clamp(part.second.restAngle - currentAngle, -0.3f, 0.3f) * 16.0f;
    }

    part.second.hinge->enableAngularMotor(true, velocity, 16.f);
  }
}

std::vector<double> WalkingRotationInputs::inputs(const Phenotype& p) const {
  btRigidBody* sternum = p.spider->parts().at("Sternum").part->rigidBody();
  mmm::vec3 rots       = ExpUtil::getEulerAngles(sternum->getOrientation());
  std::vector<double> inputs = p.previousOutput;

  if (inputs.size() == 0) {
    inputs.insert(inputs.end(), mSubstrate->m_output_coords.size(), 0);
  }

  inputs[0] = rots.x;
  inputs[1] = rots.y;
  inputs[2] = rots.z;
  inputs[3] = mmm::sin(p.duration * 3);
  inputs[4] = 1;
  inputs[5] = 1;
  inputs[6] = 1;
  inputs[7] = 1;
  inputs[8] = p.collidesWithTerrain("TarsusL1") ? 1.0 : 0.0;
  inputs[9] = p.collidesWithTerrain("TarsusL2") ? 1.0 : 0.0;
  inputs[10] = p.collidesWithTerrain("TarsusL3") ? 1.0 : 0.0;
  inputs[11] = p.collidesWithTerrain("TarsusL4") ? 1.0 : 0.0;
  inputs[12] = p.collidesWithTerrain("TarsusR1") ? 1.0 : 0.0;
  inputs[13] = p.collidesWithTerrain("TarsusR2") ? 1.0 : 0.0;
  inputs[14] = p.collidesWithTerrain("TarsusR3") ? 1.0 : 0.0;
  inputs[15] = p.collidesWithTerrain("TarsusR4") ? 1.0 : 0.0;

  size_t index = 16;
  for(auto& a : p.spider->parts()) {
    if (!a.second.active || a.second.hinge == nullptr)
      continue;

    inputs[index] = ExpUtil::normalizeAngle(a.second.hinge->getHingeAngle(), -PI, PI, 0);
    index++;
  }

  return inputs;
}
