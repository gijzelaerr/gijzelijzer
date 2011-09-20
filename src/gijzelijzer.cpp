
#include "gijzelijzer.h"
#include "gmnames.h"

const double midiScaler = (1. / 127.);
static float sawtooth[kWaveSize];
static float pulse[kWaveSize];
static float freqtab[kNumFrequencies];


AudioEffect* createEffectInstance (audioMasterCallback audioMaster)
{
        return new Gijzelijzer (audioMaster);
}


GijzelijzerProgram::GijzelijzerProgram ()
{
    // Default Program Values
    fWaveform1 = 0.f;    // saw
    fFreq1     =.0f;
    fVolume1   = .33f;

    fWaveform2 = 1.f;    // pulse
    fFreq2     = .05f;    // slightly higher
    fVolume2   = .33f;

    fVolume    = .9f;
    vst_strncpy (name, "Basic", kVstMaxProgNameLen);
}


Gijzelijzer::Gijzelijzer (audioMasterCallback audioMaster)
: AudioEffectX (audioMaster, kNumPrograms, kNumParams)
{
    // initialize programs
        programs = new GijzelijzerProgram[kNumPrograms];
    for (VstInt32 i = 0; i < 16; i++)
        channelPrograms[i] = i;

    if (programs)
        setProgram (0);

    if (audioMaster)
    {
        setNumInputs (0);    	    	// no inputs
        setNumOutputs (kNumOutputs);    // 2 outputs, 1 for each oscillator
        canProcessReplacing ();
        isSynth ();
                setUniqueID ('5864');
    }

    initProcess ();
    suspend ();
}


Gijzelijzer::~Gijzelijzer ()
{
    if (programs)
        delete[] programs;
}


void Gijzelijzer::setProgram (VstInt32 program)
{
    if (program < 0 || program >= kNumPrograms)
        return;

        GijzelijzerProgram *ap = &programs[program];
    curProgram = program;

    fWaveform1 = ap->fWaveform1;
    fFreq1     = ap->fFreq1;
    fVolume1   = ap->fVolume1;

    fWaveform2 = ap->fWaveform2;
    fFreq2     = ap->fFreq2;
    fVolume2   = ap->fVolume2;

    fVolume    = ap->fVolume;
}


void Gijzelijzer::setProgramName (char* name)
{
    vst_strncpy (programs[curProgram].name, name, kVstMaxProgNameLen);
}


void Gijzelijzer::getProgramName (char* name)
{
    vst_strncpy (name, programs[curProgram].name, kVstMaxProgNameLen);
}


void Gijzelijzer::getParameterLabel (VstInt32 index, char* label)
{
    switch (index)
    {
        case kWaveform1:
        case kWaveform2:
            vst_strncpy (label, "Shape", kVstMaxParamStrLen);
            break;

        case kFreq1:
        case kFreq2:
            vst_strncpy (label, "Hz", kVstMaxParamStrLen);
            break;

        case kVolume1:
        case kVolume2:
        case kVolume:
            vst_strncpy (label, "dB", kVstMaxParamStrLen);
            break;
    }
}


void Gijzelijzer::getParameterDisplay (VstInt32 index, char* text)
{
    text[0] = 0;
    switch (index)
    {
        case kWaveform1:
            if (fWaveform1 < .5)
                vst_strncpy (text, "Sawtooth", kVstMaxParamStrLen);
            else
                vst_strncpy (text, "Pulse", kVstMaxParamStrLen);
            break;

        case kFreq1:    	float2string (fFreq1, text, kVstMaxParamStrLen);    break;
        case kVolume1:    	dB2string (fVolume1, text, kVstMaxParamStrLen);    break;

        case kWaveform2:
            if (fWaveform2 < .5)
                vst_strncpy (text, "Sawtooth", kVstMaxParamStrLen);
            else
                vst_strncpy (text, "Pulse", kVstMaxParamStrLen);
            break;

        case kFreq2:    	float2string (fFreq2, text, kVstMaxParamStrLen);    break;
        case kVolume2:    	dB2string (fVolume2, text, kVstMaxParamStrLen);    break;
        case kVolume:    	dB2string (fVolume, text, kVstMaxParamStrLen);    break;
    }
}


void Gijzelijzer::getParameterName (VstInt32 index, char* label)
{
    switch (index)
    {
        case kWaveform1:    vst_strncpy (label, "Wave 1", kVstMaxParamStrLen);    break;
        case kFreq1:    	vst_strncpy (label, "Freq 1", kVstMaxParamStrLen);    break;
        case kVolume1:    	vst_strncpy (label, "Levl 1", kVstMaxParamStrLen);    break;
        case kWaveform2:    vst_strncpy (label, "Wave 2", kVstMaxParamStrLen);    break;
        case kFreq2:    	vst_strncpy (label, "Freq 2", kVstMaxParamStrLen);    break;
        case kVolume2:    	vst_strncpy (label, "Levl 2", kVstMaxParamStrLen);    break;
        case kVolume:    	vst_strncpy (label, "Volume", kVstMaxParamStrLen);    break;
    }
}


void Gijzelijzer::setParameter (VstInt32 index, float value)
{
        GijzelijzerProgram *ap = &programs[curProgram];
    switch (index)
    {
        case kWaveform1:    fWaveform1    = ap->fWaveform1    = value;    break;
        case kFreq1:    	fFreq1     	= ap->fFreq1    	= value;    break;
        case kVolume1:    	fVolume1    = ap->fVolume1    	= value;    break;
        case kWaveform2:    fWaveform2    = ap->fWaveform2    = value;    break;
        case kFreq2:    	fFreq2    	= ap->fFreq2    	= value;    break;
        case kVolume2:    	fVolume2    = ap->fVolume2    	= value;    break;
        case kVolume:    	fVolume    	= ap->fVolume    	= value;    break;
    }
}


float Gijzelijzer::getParameter (VstInt32 index)
{
    float value = 0;
    switch (index)
    {
        case kWaveform1:    value = fWaveform1;    break;
        case kFreq1:    	value = fFreq1;    	break;
        case kVolume1:    	value = fVolume1;    break;
        case kWaveform2:    value = fWaveform2;    break;
        case kFreq2:    	value = fFreq2;    	break;
        case kVolume2:    	value = fVolume2;    break;
        case kVolume:    	value = fVolume;    break;
    }
    return value;
}


bool Gijzelijzer::getOutputProperties (VstInt32 index, VstPinProperties* properties)
{
    if (index < kNumOutputs)
    {
        vst_strncpy (properties->label, "Vstx ", 63);
        char temp[11] = {0};
        int2string (index + 1, temp, 10);
        vst_strncat (properties->label, temp, 63);

        properties->flags = kVstPinIsActive;
        if (index < 2)
            properties->flags |= kVstPinIsStereo;    // make channel 1+2 stereo
        return true;
    }
    return false;
}


bool Gijzelijzer::getProgramNameIndexed (VstInt32 category, VstInt32 index, char* text)
{
    if (index < kNumPrograms)
    {
        vst_strncpy (text, programs[index].name, kVstMaxProgNameLen);
        return true;
    }
    return false;
}


bool Gijzelijzer::getEffectName (char* name)
{
        vst_strncpy (name, "Gijzelijzer", kVstMaxEffectNameLen);
    return true;
}


bool Gijzelijzer::getVendorString (char* text)
{
    vst_strncpy (text, "Pythonic.nl", kVstMaxVendorStrLen);
    return true;
}


bool Gijzelijzer::getProductString (char* text)
{
    vst_strncpy (text, "GIJS IN YOUR HEAD", kVstMaxProductStrLen);
    return true;
}


VstInt32 Gijzelijzer::getVendorVersion ()
{ 
    return 1000;
}


VstInt32 Gijzelijzer::canDo (char* text)
{
    if (!strcmp (text, "receiveVstEvents"))
        return 1;
    if (!strcmp (text, "receiveVstMidiEvent"))
        return 1;
    if (!strcmp (text, "midiProgramNames"))
        return 1;
    return -1;    // explicitly can't do; 0 => don't know
}


VstInt32 Gijzelijzer::getNumMidiInputChannels ()
{
    return 1; // we are monophonic
}


VstInt32 Gijzelijzer::getNumMidiOutputChannels ()
{
    return 0; // no MIDI output back to Host app
}


// midi program names:
// as an example, GM names are used here. in fact, Gijzelijzer doesn't even support
// multi-timbral operation so it's really just for demonstration.
// a 'real' instrument would have a number of voices which use the
// programs[channelProgram[channel]] parameters when it receives
// a note on message.
VstInt32 Gijzelijzer::getMidiProgramName (VstInt32 channel, MidiProgramName* mpn)
{
    VstInt32 prg = mpn->thisProgramIndex;
    if (prg < 0 || prg >= 128)
        return 0;
    fillProgram (channel, prg, mpn);
    if (channel == 9)
        return 1;
    return 128L;
}


VstInt32 Gijzelijzer::getCurrentMidiProgram (VstInt32 channel, MidiProgramName* mpn)
{
    if (channel < 0 || channel >= 16 || !mpn)
        return -1;
    VstInt32 prg = channelPrograms[channel];
    mpn->thisProgramIndex = prg;
    fillProgram (channel, prg, mpn);
    return prg;
}


void Gijzelijzer::fillProgram (VstInt32 channel, VstInt32 prg, MidiProgramName* mpn)
{
    mpn->midiBankMsb =
    mpn->midiBankLsb = -1;
    mpn->reserved = 0;
    mpn->flags = 0;

    if (channel == 9)    // drums
    {
        vst_strncpy (mpn->name, "Standard", 63);
        mpn->midiProgram = 0;
        mpn->parentCategoryIndex = 0;
    }
    else
    {
        vst_strncpy (mpn->name, GmNames[prg], 63);
        mpn->midiProgram = (char)prg;
        mpn->parentCategoryIndex = -1;    // for now

        for (VstInt32 i = 0; i < kNumGmCategories; i++)
        {
            if (prg >= GmCategoriesFirstIndices[i] && prg < GmCategoriesFirstIndices[i + 1])
            {
                mpn->parentCategoryIndex = i;
                break;
            }
        }
    }
}


VstInt32 Gijzelijzer::getMidiProgramCategory (VstInt32 channel, MidiProgramCategory* cat)
{
    cat->parentCategoryIndex = -1;    // -1:no parent category
    cat->flags = 0;    	    	    // reserved, none defined yet, zero.
    VstInt32 category = cat->thisCategoryIndex;
    if (channel == 9)
    {
        vst_strncpy (cat->name, "Drums", 63);
        return 1;
    }
    if (category >= 0 && category < kNumGmCategories)
        vst_strncpy (cat->name, GmCategories[category], 63);
    else
        cat->name[0] = 0;
    return kNumGmCategories;
}


bool Gijzelijzer::hasMidiProgramsChanged (VstInt32 channel)
{
    return false;    // updateDisplay ()
}


bool Gijzelijzer::getMidiKeyName (VstInt32 channel, MidiKeyName* key)
                                // struct will be filled with information for 'thisProgramIndex' and 'thisKeyNumber'
                                // if keyName is "" the standard name of the key will be displayed.
                                // if false is returned, no MidiKeyNames defined for 'thisProgramIndex'.
{
    // key->thisProgramIndex;    	// >= 0. fill struct for this program index.
    // key->thisKeyNumber;    	    // 0 - 127. fill struct for this key number.
    key->keyName[0] = 0;
    key->reserved = 0;    	    	// zero
    key->flags = 0;    	    	    // reserved, none defined yet, zero.
    return false;
}




void Gijzelijzer::setSampleRate (float sampleRate)
{
    AudioEffectX::setSampleRate (sampleRate);
    fScaler = (float)((double)kWaveSize / (double)sampleRate);
}


void Gijzelijzer::setBlockSize (VstInt32 blockSize)
{
    AudioEffectX::setBlockSize (blockSize);
    // you may need to have to do something here...
}


void Gijzelijzer::initProcess ()
{
    fPhase1 = fPhase2 = 0.f;
    fScaler = (float)((double)kWaveSize / 44100.);    // we don't know the sample rate yet
    noteIsOn = false;
    currentDelta = currentNote = currentDelta = 0;
    VstInt32 i;

    // make waveforms
    VstInt32 wh = kWaveSize / 4;    // 1:3 pulse
    for (i = 0; i < kWaveSize; i++)
    {
        sawtooth[i] = (float)(-1. + (2. * ((double)i / (double)kWaveSize)));
        pulse[i] = (i < wh) ? -1.f : 1.f;
    }

    // make frequency (Hz) table
    double k = 1.059463094359;    // 12th root of 2
    double a = 6.875;    // a
    a *= k;    // b
    a *= k;    // bb
    a *= k;    // c, frequency of midi note 0
    for (i = 0; i < kNumFrequencies; i++)    // 128 midi notes
    {
        freqtab[i] = (float)a;
        a *= k;
    }
}


void Gijzelijzer::processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames)
{
    float* out1 = outputs[0];
    float* out2 = outputs[1];

    if (noteIsOn)
    {
        float baseFreq = freqtab[currentNote & 0x7f] * fScaler;
        float freq1 = baseFreq + fFreq1;    // not really linear...
        float freq2 = baseFreq + fFreq2;
        float* wave1 = (fWaveform1 < .5) ? sawtooth : pulse;
        float* wave2 = (fWaveform2 < .5) ? sawtooth : pulse;
        float wsf = (float)kWaveSize;
        float vol = (float)(fVolume * (double)currentVelocity * midiScaler);
        VstInt32 mask = kWaveSize - 1;

        if (currentDelta > 0)
        {
            if (currentDelta >= sampleFrames)    // future
            {
                currentDelta -= sampleFrames;
                return;
            }
            memset (out1, 0, currentDelta * sizeof (float));
            memset (out2, 0, currentDelta * sizeof (float));
            out1 += currentDelta;
            out2 += currentDelta;
            sampleFrames -= currentDelta;
            currentDelta = 0;
        }

        // loop
        while (--sampleFrames >= 0)
        {
            // this is all very raw, there is no means of interpolation,
            // and we will certainly get aliasing due to non-bandlimited
            // waveforms. don't use this for serious projects...
            (*out1++) = wave1[(VstInt32)fPhase1 & mask] * fVolume1 * vol;
            (*out2++) = wave2[(VstInt32)fPhase2 & mask] * fVolume2 * vol;
            fPhase1 += freq1;
            fPhase2 += freq2;
        }
    }
    else
    {
        memset (out1, 0, sampleFrames * sizeof (float));
        memset (out2, 0, sampleFrames * sizeof (float));
    }
}


VstInt32 Gijzelijzer::processEvents (VstEvents* ev)
{
    for (VstInt32 i = 0; i < ev->numEvents; i++)
    {
        if ((ev->events[i])->type != kVstMidiType)
            continue;

        VstMidiEvent* event = (VstMidiEvent*)ev->events[i];
        char* midiData = event->midiData;
        VstInt32 status = midiData[0] & 0xf0;    // ignoring channel
        if (status == 0x90 || status == 0x80)    // we only look at notes
        {
            VstInt32 note = midiData[1] & 0x7f;
            VstInt32 velocity = midiData[2] & 0x7f;
            if (status == 0x80)
                velocity = 0;    // note off by velocity 0
            if (!velocity && (note == currentNote))
                noteOff ();
            else
                noteOn (note, velocity, event->deltaFrames);
        }
        else if (status == 0xb0)
        {
            if (midiData[1] == 0x7e || midiData[1] == 0x7b)    // all notes off
                noteOff ();
        }
        event++;
    }
    return 1;
}


void Gijzelijzer::noteOn (VstInt32 note, VstInt32 velocity, VstInt32 delta)
{
    currentNote = note;
    currentVelocity = velocity;
    currentDelta = delta;
    noteIsOn = true;
    fPhase1 = fPhase2 = 0;
}


void Gijzelijzer::noteOff ()
{
    noteIsOn = false;
}
