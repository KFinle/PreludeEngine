#pragma once
#include <vector>

#include "Targets/TimingTargetMode.h"

struct EventSequence;

namespace Rhythm
{
    ////////////////////
    // Timing Targets //
    ////////////////////
    void CollectTimingTargets(const EventSequence& seq,
                              TimingTargetMode mode,
                              float current_beat,
                              float lookahead_beats,
                              std::vector<float>& out_targets);
}
