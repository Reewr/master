#include "WalkingPunished.hpp"

#include "ExperimentUtil.hpp"

#include "../Learning/Phenotype.hpp"
#include "../Learning/Substrate.hpp"

#include <btBulletDynamicsCommon.h>

const float PI = mmm::constants<float>::pi;

WalkingPunished::WalkingPunished() : Experiment("WalkingPunished") {

  mParameters.numActivates = 8;
  mParameters.experimentDuration = 30;
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
  Fitness("Vibrating",
            "Fitness based how little it vibrates with the legs",
            [](const Phenotype&, float, float) -> float {
              return 0;
            },
            [](const Phenotype& p, float current, float duration) -> float {
              size_t numUpdates = p.tmp.size();
              size_t numJoints  = numUpdates == 0 ? 0 : p.tmp[0].size();

              for(size_t i = 0; i < numJoints; ++i) {
                float dir   = 0.0;
                float freq  = 0.0;

                for(size_t j = 0; j < numUpdates; j++) {
                  float currentDir = p.tmp[j][i];

                  if (currentDir != dir)
                    freq += 1;

                  dir = currentDir;
                }

                float avgHz = freq / duration;

                if (avgHz > 10)
                  current -= 1 / float(numJoints);
              }

              return 1.0f + current;
            }),
  Fitness("Off Center",
            "Fitness based how much off center it is",
            [](const Phenotype& p, float current, float) -> float {
              const btRigidBody* sternum = p.rigidBody("Sternum");
              float xpos = sternum->getCenterOfMassPosition().x();
              return current + ExpUtil::score(1.0, xpos, 0.1);
            },
            [](const Phenotype&, float current, float duration) -> float {
              return current / (duration * 60);
            }),

  Fitness("Off Center Rotation",
            "Fitness based how much rotation off center it is",
            [](const Phenotype& p, float current, float) -> float {
              const btRigidBody* sternum = p.rigidBody("Sternum");
              float yrot = ExpUtil::getEulerAngles(sternum->getOrientation()).y;
              yrot += mmm::radians(90);
              return current + ExpUtil::score(1.0, yrot, mmm::radians(10));
            },
            [](const Phenotype&, float current, float duration) -> float {
              return current / (duration * 60);
            }),

    Fitness("Colliding",
            "If the spider falls, punish it",
            [](const Phenotype& phenotype, float current, float dt) -> float {

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
                phenotype.collidesWithTerrain("TrochanterL4"))
                phenotype.kill();

              return current;
            })
  };

  mSubstrate = createDefaultSubstrate();
  mSubstrate->m_hidden_nodes_activation = NEAT::ActivationFunction::TANH;
  mSubstrate->m_output_nodes_activation = NEAT::ActivationFunction::TANH;
  mSubstrate->m_max_weight_and_bias = 3.0;

  NEAT::Parameters params = getDefaultParameters();
  params.MutateWeightsProb = 0.9;
  params.WeightMutationMaxPower = 0.2;
  params.WeightReplacementMaxPower = 1.0;
  params.MutateNeuronActivationTypeProb = 0.03;
  params.ActivationFunction_SignedSigmoid_Prob = 1.0;
  params.ActivationFunction_Tanh_Prob = 1.0;
  params.ActivationFunction_Linear_Prob = 1.0;

  NEAT::Genome genome(0,
                      mSubstrate->GetMinCPPNInputs(),
                      0,
                      mSubstrate->GetMinCPPNOutputs(),
                      false,
                      NEAT::ActivationFunction::TANH_CUBIC,
                      NEAT::ActivationFunction::SIGNED_SINE,
                      0,
                      params);

  mPopulation = new NEAT::Population(genome, params, true, params.MaxWeight, time(0));
}

WalkingPunished::~WalkingPunished() {
  delete mPopulation;
  delete mSubstrate;
}

float WalkingPunished::mergeFitnessValues(const mmm::vec<9>& fitness) const {
  return fitness[0] * fitness[1] * fitness[2] * fitness[3];
}

void WalkingPunished::outputs(Phenotype&                 p,
                                  const std::vector<double>& outputs) const {
  size_t index = 16;
  for(auto& part : p.spider->parts()) {
    if (part.second.hinge == nullptr)
      continue;

    float currentAngle = part.second.hinge->getHingeAngle();
    float velocity;

    if (part.second.active) {
      float zero   = (part.second.hinge->getUpperLimit() + part.second.hinge->getLowerLimit()) * 0.5;
      float output = ExpUtil::denormalizeAngle(outputs[index],
                                               part.second.hinge->getLowerLimit(),
                                               part.second.hinge->getUpperLimit(),
                                               zero);
      velocity = output - currentAngle;

      if (p.tmp.size() <= index - 16)
        p.tmp.push_back({});
      p.tmp[index-16].push_back(output > currentAngle ? 1.0 : 0.0);

      index++;
    } else {
      velocity = mmm::clamp(part.second.restAngle - currentAngle, -0.3f, 0.3f) * 16.0f;
    }

    part.second.hinge->enableAngularMotor(true, velocity, 16.f);
  }
}

std::vector<double> WalkingPunished::inputs(const Phenotype& p) const {
  btRigidBody* sternum = p.spider->parts().at("Sternum").part->rigidBody();
  mmm::vec3 rots       = ExpUtil::getEulerAngles(sternum->getOrientation());
  std::vector<double> inputs = p.previousOutput;

  if (inputs.size() == 0) {
    inputs.insert(inputs.end(), mSubstrate->m_output_coords.size(), 0);
  }

  inputs[0] = ExpUtil::normalizeAngle(rots.x, -PI, PI, 0);
  inputs[1] = ExpUtil::normalizeAngle(rots.y + mmm::degrees(90), -PI, PI, 0);
  inputs[2] = ExpUtil::normalizeAngle(rots.z, -PI, PI, 0);
  inputs[3] = mmm::sin(p.duration * 2);
  inputs[4] = 1;
  inputs[5] = 1;
  inputs[6] = 1;
  inputs[7] = mmm::cos(p.duration * 2);
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
    float zero  = (a.second.hinge->getUpperLimit() + a.second.hinge->getLowerLimit()) * 0.5;
    float angle = ExpUtil::normalizeAngle(a.second.hinge->getHingeAngle(),
                                          a.second.hinge->getLowerLimit(),
                                          a.second.hinge->getUpperLimit(),
                                          zero);

    inputs[index] = angle;
    index++;
  }

  return inputs;
}
