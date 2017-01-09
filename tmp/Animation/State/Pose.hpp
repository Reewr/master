#ifndef ANIMATION_POSE_HPP
#define ANIMATION_POSE_HPP

#include <Animation/State/State.hpp>

namespace Animation {

  template <size_t n>
  struct Pose : State<n> {

    constexpr Pose (Frame<n>&& frame);
    constexpr Pose (const Frame<n>& frame);

    constexpr State<n>* copy      ();
    constexpr float     length    ();
              State<n>* advance   (float);
    constexpr Frame<n>  current   ();
    constexpr Frame<n>  at        (float);

    private:
    const Frame<n> frame;
  };

  #include "Pose.tpp"
}

#endif