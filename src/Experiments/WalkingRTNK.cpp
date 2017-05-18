#include "WalkingRTNK.hpp"

#include "ExperimentUtil.hpp"

#include "../Learning/Phenotype.hpp"
#include "../Learning/Substrate.hpp"

#include <btBulletDynamicsCommon.h>

const float PI = mmm::constants<float>::pi;

WalkingRTNK::WalkingRTNK() : Experiment("WalkingRTNK") {

  mParameters.numActivates = 8;
  mParameters.experimentDuration = 15;
  mFitnessFunctions =
  { Fitness("MovementZ",
            "Fitness based on movement in positive z direction.",
            [](const Phenotype& p, float current, float) -> float {
              const btRigidBody* sternum = p.rigidBody("Sternum");
              const btVector3& massPos = sternum->getCenterOfMassPosition();
              return mmm::max(current, massPos.z());
            }),

    Fitness("MovementX",
            "Fitness based on movement in positive z direction.",
            [](const Phenotype& p, float current, float) -> float {
              const btRigidBody* sternum = p.rigidBody("Sternum");
              const btVector3& massPos = sternum->getCenterOfMassPosition();
              return mmm::max(current, mmm::abs(massPos.x()));
            }),

    Fitness("Vibrating",
            "Fitness based how little it vibrates with the legs",
            [](const Phenotype& p, float, float) -> float {
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

                if (avgHz > 5)
                  current -= 1 / float(numJoints);
              }

              return 1.0f + current;
            }),
  };


  mSubstrate = createDefaultSubstrate();
  NEAT::Parameters params = getDefaultParameters();
  params.ActivationFunction_SignedSigmoid_Prob = 1.0;
  params.ActivationFunction_UnsignedSigmoid_Prob = 0.0;
  params.ActivationFunction_Tanh_Prob = 1.0;
  params.ActivationFunction_TanhCubic_Prob = 1.0;
  params.ActivationFunction_SignedStep_Prob = 1.0;
  params.ActivationFunction_UnsignedStep_Prob = 0.0;
  params.ActivationFunction_SignedGauss_Prob = 1.0;
  params.ActivationFunction_UnsignedGauss_Prob = 0.0;
  params.ActivationFunction_Abs_Prob = 1.0;
  params.ActivationFunction_SignedSine_Prob = 1.0;
  params.ActivationFunction_UnsignedSine_Prob = 0.0;
  params.ActivationFunction_Linear_Prob = 1.0;
  params.ActivationFunction_Relu_Prob = 0.0;
  params.ActivationFunction_Softplus_Prob = 0.0;

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

WalkingRTNK::~WalkingRTNK() {
  delete mPopulation;
  delete mSubstrate;
}

float WalkingRTNK::mergeFitnessValues(const mmm::vec<9>& f) const {
  return mmm::max(f.x - f.y, 0.f) * f.z;
}

void WalkingRTNK::outputs(Phenotype&                 p,
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

std::vector<double> WalkingRTNK::inputs(const Phenotype& p) const {
  btRigidBody* sternum = p.spider->parts().at("Sternum").part->rigidBody();
  mmm::vec3 rots       = ExpUtil::getEulerAngles(sternum->getOrientation());
  std::vector<double> inputs = p.previousOutput;

  if (inputs.size() == 0) {
    inputs.insert(inputs.end(), mSubstrate->m_output_coords.size(), 0);
  }

  inputs[0] = rots.x;
  inputs[1] = rots.y;
  inputs[2] = rots.z;
  inputs[3] = mmm::sin(p.duration * 2);
  // inputs[4] = 1;
  // inputs[5] = 1;
  // inputs[6] = 1;
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
