#include "StandingAngles.hpp"
#include "ExperimentUtil.hpp"

#include "../Learning/Phenotype.hpp"
#include "../Learning/Substrate.hpp"

#include <btBulletDynamicsCommon.h>

StandingAngles::StandingAngles() : Experiment("StandingAngles") {

  mParameters.flatMode = true;

  mParameters.numActivates = 8;
  mParameters.experimentDuration = 30;
  mFitnessFunctions =
  { Fitness("Angles    ",
            "Fitness based on rest angles.",
            [](const Phenotype&, float, float) -> float {
              return 0.f;
            },
            [](const Phenotype& p, float current, float duration) -> float {
              size_t updates = p.tmp.size();

              size_t i = 0;
              for (auto& part : p.spider->parts()) {
                if (!part.second.active || part.second.hinge == nullptr)
                  continue;

                float c = 0.f;

                for (size_t j = 0; j < updates; ++j) {
                  float x = mmm::abs(p.tmp[j][i] - part.second.restAngle);

                  if (x < 0.025)
                    c += 2.f;
                  else if (x < 0.05f)
                    c += 1.f;
                  else if (x < 0.1f)
                    c += 0.5f;
                  else
                    c = 0.f;

                  current += c;
                }

                i++;
              }

              return current;
            }),

    Fitness("Lifespan  ",
            "Fitness based on lifespan.",
            [](const Phenotype&, float, float) -> float {
              return 0.f;
            },
            [](const Phenotype&, float, float duration) -> float {
              return duration;
            }),

    Fitness("Colliding",
            "If the spider falls, stop the simulation",
            [](const Phenotype& phenotype, float current, float) -> float {

              if (phenotype.duration < 2.f)
                return current;

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
  params.ActivationFunctionDiffCoeff = 0.0;
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
}

StandingAngles::~StandingAngles() {
  delete mPopulation;
  delete mSubstrate;
}

float StandingAngles::mergeFitnessValues(const mmm::vec<9>& fitness) const {
  return mmm::sum(fitness);
}

void StandingAngles::outputs(Phenotype&                 p,
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

      // if (p.tmp.size() <= index - 16)
      //   p.tmp.push_back({});
      // p.tmp[index-16].push_back(output > currentAngle ? 1.0 : 0.0);

      index++;
    } else {
      velocity = mmm::clamp(part.second.restAngle - currentAngle, -0.3f, 0.3f) * 16.0f;
    }

    part.second.hinge->enableAngularMotor(true, velocity, 16.f);
  }
}

std::vector<double> StandingAngles::inputs(const Phenotype& p) const {
  btRigidBody* sternum = p.spider->parts().at("Sternum").part->rigidBody();
  mmm::vec3 rots       = ExpUtil::getEulerAngles(sternum->getOrientation());
  std::vector<double> inputs = p.previousOutput;

  if (inputs.size() == 0) {
    inputs.insert(inputs.end(), mSubstrate->m_output_coords.size(), 0);
  }

  inputs[0] = rots.x;
  inputs[1] = rots.y;
  inputs[2] = rots.z;
  // inputs[3] = 1; // mmm::sin(p.duration * 2);
  // inputs[4] = 1; // mmm::cos(p.duration * 2);
  // inputs[5] = 1;
  // inputs[6] = 1;
  // inputs[7] = 1;
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

void StandingAngles::postUpdate(const Phenotype& p) const {
  auto& parts = p.spider->parts();

  std::vector<float> xs;
  for (auto& part : parts) {
    if (!part.second.active || part.second.hinge == nullptr)
      continue;

    auto angle = part.second.hinge->getHingeAngle();
    auto lower = part.second.hinge->getLowerLimit();
    auto upper = part.second.hinge->getUpperLimit();
    auto rest  = part.second.restAngle;

    xs.push_back(ExpUtil::normalizeAngle(angle, lower, upper, rest));
  }

  p.tmp.push_back(xs);
}
