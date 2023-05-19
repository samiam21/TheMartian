#include "TheMartian.h"

/**
 * Audio callback to process each enabled effect
 */
void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        float wet = in[0][i];

        if (shimmerOn)
        {
            // Compress the signal prior to shifting the pitch for a smoother sound
            wet = compressor.Process(wet);

            // Add the octave up pitch shift effect for the shimmer
            wet = shifter.Process(wet);

            // Add a small chorus effect to the pitch shift for a "strings-esque" sound
            wet = chorus.Process(wet);
        }

        // Add the reverb effect
        wet = reverb.Process(wet);

        // Add the level boost and send out
        out[0][i] = boost.Process(wet);
    }
}

void InitializeControls()
{
    // Initialize the ADC
    AdcChannelConfig adcConfig[MAX_KNOBS];
    adcConfig[KNOB_1_CHN].InitSingle(hw->GetPin(effectPotPin1));
    adcConfig[KNOB_2_CHN].InitSingle(hw->GetPin(effectPotPin2));
    adcConfig[KNOB_3_CHN].InitSingle(hw->GetPin(effectPotPin3));
    adcConfig[KNOB_4_CHN].InitSingle(hw->GetPin(effectPotPin4));
    adcConfig[KNOB_5_CHN].InitSingle(hw->GetPin(effectPotPin5));
    adcConfig[KNOB_6_CHN].InitSingle(hw->GetPin(effectPotPin6));
    hw->adc.Init(adcConfig, MAX_KNOBS);
    hw->adc.Start();

    // TODO: Find a better way to do this?
    // Give the ADC time to start up
    System::Delay(500);

    // Initialize the effect knobs
    boost.ConfigureKnobPositions(KNOB_6_CHN);
    reverb.ConfigureKnobPositions(KNOB_1_CHN, KNOB_2_CHN, KNOB_3_CHN);
    shifter.ConfigureKnobPositions(KNOB_4_CHN, KNOB_NO_CHN, KNOB_NO_CHN, KNOB_NO_CHN);
    compressor.ConfigureKnobPositions(KNOB_NO_CHN, KNOB_NO_CHN, KNOB_NO_CHN, KNOB_NO_CHN);
    chorus.ConfigureKnobPositions(KNOB_NO_CHN, KNOB_NO_CHN, KNOB_NO_CHN, KNOB_NO_CHN);

    // reverb.ConfigureKnobPositions(KNOB_NO_CHN, KNOB_NO_CHN, KNOB_NO_CHN);
    // shifter.ConfigureKnobPositions(KNOB_NO_CHN, KNOB_NO_CHN, KNOB_NO_CHN, KNOB_NO_CHN);
    // compressor.ConfigureKnobPositions(KNOB_NO_CHN, KNOB_NO_CHN, KNOB_NO_CHN, KNOB_NO_CHN);
    // chorus.ConfigureKnobPositions(KNOB_1_CHN, KNOB_2_CHN, KNOB_3_CHN, KNOB_4_CHN);

    // Initialize the toggles
    shimmerToggle.Init(hw->GetPin(effectTogglePin2));
}

void InitializeEffects()
{
    // Initialize the boost effect
    boost.SetBoostMinMax(BOOST_MIN, BOOST_MAX);
    boost.Setup(hw);

    // Initialize the reverb effect
    reverb.SetMinMaxDecay(DECAY_MIN, DECAY_MAX);
    reverb.SetMinMaxMix(VERB_MIX_MIN, VERB_MIX_MAX);
    reverb.ShouldReverseTonePot(true);
    reverb.Setup(hw);

    // Initialize the pitch shifter
    shifter.SetMinMaxMix(PITCH_MIX_MIN, PITCH_MIX_MAX);
    shifter.Setup(hw);

    // Initialize the compressor
    compressor.Setup(hw);

    // Initialize the chorus
    chorus.Setup(hw);
}

int main(void)
{
    // Initialize the hardware
    hw->Init();

    // Update the block size and sample rate to minimize noise
    hw->SetAudioBlockSize(DAISY_BLOCKSIZE);
    hw->SetAudioSampleRate(DAISY_SAMPLE_RATE);

    // Initialize the input controls
    InitializeControls();

    // Initialize the effect objects
    InitializeEffects();

    // Start the audio processing
    debugPrintln(hw, "Starting Audio");
    hw->StartAudio(AudioCallback);

    // Turn on the onboard LED
    hw->SetLed(true);

    while (1)
    {
        // Check the shimmer toggle
        shimmerOn = !shimmerToggle.ReadToggle();

        // Run the effect loop functions
        shifter.Loop(true);
        reverb.Loop(true);
        boost.Loop(true);
        compressor.Loop(true);
        chorus.Loop(true);
    }
}
