#ifndef THE_MARTIAN_H
#define THE_MARTIAN_H

#include "../include/PedalConfig.h"
#include "../lib/DaisyInputs/Knob.h"
#include "../lib/DaisyInputs/SimpleToggle.h"
#include "../lib/DaisyEffects/CleanBoost.h"
#include "../lib/DaisyEffects/Reverb.h"

// Use the daisy namespace to prevent having to type
// daisy:: before all libdaisy functions
using namespace daisy;

// Declare a DaisySeed object called hw
DaisySeed *hw;

// Declare the effects objects being used
CleanBoost boost;
Reverb reverb;

// Declare effect min and maxes (reversed because pots are backwards)
const float BOOST_MIN = 5.f;
const float BOOST_MAX = 0.5f;
const float DECAY_MIN = 0.99f;
const float DECAY_MAX = 0.89f;
const float MIX_MIN = 1.f;
const float MIX_MAX = 0.0f;

/**
 * Audio callback to process each enabled effect
 */
void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size);

/**
 * Initializes the input controls
 */
void InitializeControls();

/**
 * Initializes the effects
 */
void InitializeEffects();

#endif