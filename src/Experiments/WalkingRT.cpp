#include "WalkingRT.hpp"

#include "ExperimentUtil.hpp"

#include "../Learning/Phenotype.hpp"
#include "../Learning/Substrate.hpp"

#include <btBulletDynamicsCommon.h>

const float PI = mmm::constants<float>::pi;

WalkingRT::WalkingRT() : Experiment("WalkingRT") {

  mParameters.numActivates = 8;
  mParameters.experimentDuration = 30;
  mFitnessFunctions =
  { Fitness("Movement",
            "Fitness based on movement in positive z direction.",
            [](const Phenotype& p, float current, float) -> float {
              const btRigidBody* sternum = p.rigidBody("Sternum");
              const btVector3& massPos = sternum->getCenterOfMassPosition();
              return mmm::max(current, massPos.z());
            }),

    Fitness("TEST      ",
            "...",
            [](const Phenotype& p, float current, float) -> float {
              return current;
            }, [](const Phenotype& p, float current, float) -> float {
              size_t numUpdates = p.tmp.size() / 3;

              const float dt = 1.f / 60.f;

              float x = 0.f;
              float y = 0.f;

              for (size_t i = 0; i < 8; ++i) {

                float prev_trochan = 0.f;

                for (size_t j = 0; j < numUpdates; ++j) {
                  float trochan = p.tmp[j*3+1][i];
                  float contact = p.tmp[j*3+2][i];

                  if (mmm::sign(trochan - prev_trochan) == contact)
                    x += 1.f;

                  y += 1.f;
                  prev_trochan = trochan;
                }
              }

              return x / y;
            }),

    Fitness("Vibrating",
            "Fitness based how little it vibrates with the legs",
            [](const Phenotype& p, float, float) -> float {
              return 0;
            },
            [](const Phenotype& p, float current, float duration) -> float {
              size_t numUpdates = p.tmp.size() / 3;
              size_t numJoints  = numUpdates == 0 ? 0 : p.tmp[0].size();

              for(size_t i = 0; i < numJoints; ++i) {
                float dir   = 0.0;
                float freq  = 0.0;

                for(size_t j = 0; j < numUpdates; j++) {
                  float currentDir = p.tmp[j*3][i];

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

    Fitness("Colliding ",
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

WalkingRT::~WalkingRT() {
  delete mPopulation;
  delete mSubstrate;
}

float WalkingRT::mergeFitnessValues(const mmm::vec<9>& fitness) const {
  return fitness.x * fitness.y * fitness.z;
}

void WalkingRT::outputs(Phenotype&                 p,
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

std::vector<double> WalkingRT::inputs(const Phenotype& p) const {
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

void WalkingRT::postUpdate(const Phenotype& p) const {
  auto& parts = p.spider->parts();

  auto& tl1 = parts.at("TrochanterL1");
  auto& tl2 = parts.at("TrochanterL2");
  auto& tl3 = parts.at("TrochanterL3");
  auto& tl4 = parts.at("TrochanterL4");
  auto& tr1 = parts.at("TrochanterR1");
  auto& tr2 = parts.at("TrochanterR2");
  auto& tr3 = parts.at("TrochanterR3");
  auto& tr4 = parts.at("TrochanterR4");

  p.tmp.push_back(std::vector<float>{
    ExpUtil::normalizeAngle(tl1.hinge->getHingeAngle(), tl1.hinge->getLowerLimit(), tl1.hinge->getUpperLimit(), tl1.restAngle),
    ExpUtil::normalizeAngle(tl2.hinge->getHingeAngle(), tl2.hinge->getLowerLimit(), tl2.hinge->getUpperLimit(), tl2.restAngle),
    ExpUtil::normalizeAngle(tl3.hinge->getHingeAngle(), tl3.hinge->getLowerLimit(), tl3.hinge->getUpperLimit(), tl3.restAngle),
    ExpUtil::normalizeAngle(tl4.hinge->getHingeAngle(), tl4.hinge->getLowerLimit(), tl4.hinge->getUpperLimit(), tl4.restAngle),
    ExpUtil::normalizeAngle(tr1.hinge->getHingeAngle(), tr1.hinge->getLowerLimit(), tr1.hinge->getUpperLimit(), tr1.restAngle),
    ExpUtil::normalizeAngle(tr2.hinge->getHingeAngle(), tr2.hinge->getLowerLimit(), tr2.hinge->getUpperLimit(), tr2.restAngle),
    ExpUtil::normalizeAngle(tr3.hinge->getHingeAngle(), tr3.hinge->getLowerLimit(), tr3.hinge->getUpperLimit(), tr3.restAngle),
    ExpUtil::normalizeAngle(tr4.hinge->getHingeAngle(), tr4.hinge->getLowerLimit(), tr4.hinge->getUpperLimit(), tr4.restAngle)
  });

  p.tmp.push_back(std::vector<float>{
    p.collidesWithTerrain("TarsusL1") ? -1.f : 1.f,
    p.collidesWithTerrain("TarsusL2") ? -1.f : 1.f,
    p.collidesWithTerrain("TarsusL3") ? -1.f : 1.f,
    p.collidesWithTerrain("TarsusL4") ? -1.f : 1.f,
    p.collidesWithTerrain("TarsusR1") ? -1.f : 1.f,
    p.collidesWithTerrain("TarsusR2") ? -1.f : 1.f,
    p.collidesWithTerrain("TarsusR3") ? -1.f : 1.f,
    p.collidesWithTerrain("TarsusR4") ? -1.f : 1.f
  });
}
