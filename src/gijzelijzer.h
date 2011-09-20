#ifndef __gijzelijzer__
#define __gijzelijzer__


#include "public.sdk/source/vst2.x/audioeffectx.h"

enum
{
    kNumFrequencies = 128,    // 128 midi notes
    kWaveSize = 4096    	// samples (must be power of 2 here)
};

enum
{
    // Global
    kNumPrograms = 128,
    kNumOutputs = 2,

    // Parameters Tags
    kWaveform1 = 0,
    kFreq1,
    kVolume1,

    kWaveform2,
    kFreq2,
    kVolume2,

    kVolume,
    
    kNumParams
};


class GijzelijzerProgram
{
friend class Gijzelijzer;
public:
    GijzelijzerProgram ();
    ~GijzelijzerProgram () {}

private:
    float fWaveform1;
    float fFreq1;
    float fVolume1;

    float fWaveform2;
    float fFreq2;
    float fVolume2;

    float fVolume;
    char name[kVstMaxProgNameLen+1];
};


class Gijzelijzer : public AudioEffectX
{
public:
    Gijzelijzer (audioMasterCallback audioMaster);
    ~Gijzelijzer ();

    virtual void processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames);
    virtual VstInt32 processEvents (VstEvents* events);

    virtual void setProgram (VstInt32 program);
    virtual void setProgramName (char* name);
    virtual void getProgramName (char* name);
    virtual bool getProgramNameIndexed (VstInt32 category, VstInt32 index, char* text);

    virtual void setParameter (VstInt32 index, float value);
    virtual float getParameter (VstInt32 index);
    virtual void getParameterLabel (VstInt32 index, char* label);
    virtual void getParameterDisplay (VstInt32 index, char* text);
    virtual void getParameterName (VstInt32 index, char* text);
    
    virtual void setSampleRate (float sampleRate);
    virtual void setBlockSize (VstInt32 blockSize);
    
    virtual bool getOutputProperties (VstInt32 index, VstPinProperties* properties);
    	
    virtual bool getEffectName (char* name);
    virtual bool getVendorString (char* text);
    virtual bool getProductString (char* text);
    virtual VstInt32 getVendorVersion ();
    virtual VstInt32 canDo (char* text);

    virtual VstInt32 getNumMidiInputChannels ();
    virtual VstInt32 getNumMidiOutputChannels ();

    virtual VstInt32 getMidiProgramName (VstInt32 channel, MidiProgramName* midiProgramName);
    virtual VstInt32 getCurrentMidiProgram (VstInt32 channel, MidiProgramName* currentProgram);
    virtual VstInt32 getMidiProgramCategory (VstInt32 channel, MidiProgramCategory* category);
    virtual bool hasMidiProgramsChanged (VstInt32 channel);
    virtual bool getMidiKeyName (VstInt32 channel, MidiKeyName* keyName);

private:
    float fWaveform1;
    float fFreq1;
    float fVolume1;
    float fWaveform2;
    float fFreq2;
    float fVolume2;
    float fVolume;    
    float fPhase1, fPhase2;
    float fScaler;

    GijzelijzerProgram* programs;
    VstInt32 channelPrograms[16];

    VstInt32 currentNote;
    VstInt32 currentVelocity;
    VstInt32 currentDelta;
    bool noteIsOn;

    void initProcess ();
    void noteOn (VstInt32 note, VstInt32 velocity, VstInt32 delta);
    void noteOff ();
    void fillProgram (VstInt32 channel, VstInt32 prg, MidiProgramName* mpn);
};

#endif
