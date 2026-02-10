#pragma once

namespace Rhythm
{
    // TimingTargetMode
    // what rhythm pattern we're following
    enum class TimingTargetMode
    {
        Barline,
        Snare,
        KickAndSnare,
        Melody,

        // used for testing
        All,

        // game defaults to kick
        Kick
    };
}
