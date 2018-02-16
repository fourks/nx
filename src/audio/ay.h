//----------------------------------------------------------------------------------------------------------------------
// AY-3-8912 emulation
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include <types.h>
#include <config.h>

class AYChip
{
public:
    enum class Type
    {
        AY,
        YM,
    };

    enum class Register
    {
        PitchA_Fine,                // 8 bits       0
        PitchA_Coarse,              // 4 bits       1
        PitchB_Fine,                //              2
        PitchB_Coarse,              //              3
        PitchC_Fine,                //              4
        PitchC_Coarse,              //              5
        PitchNoise,                 // 5 bits       6
        Mixer,                      // 8 bits       7
        VolumeA,                    // 4 bits       8
        VolumeB,                    // 4 bits       9
        VolumeC,                    // 4 bits       10
        EnvelopeDuration_Fine,      // 8 bits       11
        EnvelopeDuration_Coarse,    // 8 bits       12
        EnvelopeShape,              // 4 bits       13
        IoPortA,                    // 8 bits       14
    };

    AYChip();

    void reset(Type type, int freq, int chans, int bits);
    void setRegs(vector<u8> regs);
    void setReg(Register reg, u8 value);

    void play(void* outBuf, size_t numFrames);

private:
    struct RegisterInfo
    {
        int     m_tone[3];          // Registers 0-5
        int     m_volume[3];        // Registers 8-10
        bool    m_envelope[3];      // Registers 8-10 (bit 4)
        bool    m_mixerTone[3];     // Register 7 (bits 0-3)
        bool    m_mixerNoise[3];    // Register 7 (bits 4-6)
        int     m_noise;            // Register 6
        int     m_envFreq;          // Registers 11-12
        int     m_envType;          // Register 13
    };

    //
    // Sound state
    //
    enum Counter
    {
        CounterA,
        CounterB,
        CounterC,
        CounterNoise,
        CounterEnvelope,
    };

    RegisterInfo    m_regs;
    int*            m_table;
    int             m_freq;
    int             m_numChannels;
    int             m_sampleSize;       // In bits

    int             m_state[4];         // State for A, B, C & noise.
    int             m_counters[5];      // Counter for A, B, C, noise and envelope.
    int             m_envX;
};
