#pragma once
#include <vector>
#include "../Events/EventSequence.h"
#include "RenderSettings.h"

/////////////////////////////
// Event Sequence Renderer //
//////////////////////////////////////////////////////////
// This is where we convert an EventSequence into       //
// a mixed sample buffer containing all musical notes   //
// defined in the sequence.                             //
//////////////////////////////////////////////////////////
// Final step before rendering the buffer to a WAV      //
// file for playback.                                   //
//////////////////////////////////////////////////////////
class EventSequenceRenderer
{
public:
    // renders an EventSequence into a float buffer
    static std::vector<float> RenderToBuffer(const EventSequence& sequence, const RenderSettings& settings);
};
