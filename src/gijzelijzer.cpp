
#include <stdio.h>
#include <string.h>

#ifndef __gijzelijzer__
#include "gijzelijzer.h"
#endif


GijzelijzerProgram::GijzelijzerProgram ()
{
	// default Program Values
	fDelay = 0.5;
	fFeedBack = 0.5;
	fOut = 0.75;

	strcpy (name, "Init");
}


Gijzelijzer::Gijzelijzer (audioMasterCallback audioMaster)
	: AudioEffectX (audioMaster, kNumPrograms, kNumParams)
{
    VstTimeInfo* vstTimeInfo = getTimeInfo(kVstTempoValid + kVstTransportPlaying);

	// init
	size = vstTimeInfo->sampleRate;
    fTempo = vstTimeInfo->tempo;
	cursor = 0;
	delay = 0;
	buffer = new float[size];
	
    programs = new GijzelijzerProgram[numPrograms];
	fDelay = fFeedBack = fOut = 0;

	if (programs)
		setProgram (0);

	setNumInputs (1);	// mono input
	setNumOutputs (2);	// stereo output

    setUniqueID ('5864');

    resume ();
}


Gijzelijzer::~Gijzelijzer ()
{
	if (buffer)
		delete[] buffer;
	if (programs)
		delete[] programs;
}


void Gijzelijzer::setProgram (VstInt32 program)
{
    GijzelijzerProgram* ap = &programs[program];

	curProgram = program;
	setParameter (kDelay, ap->fDelay);	
	setParameter (kFeedBack, ap->fFeedBack);
	setParameter (kOut, ap->fOut);
}


void Gijzelijzer::setDelay (float fdelay)
{
	fDelay = fdelay;
	programs[curProgram].fDelay = fdelay;
    //cursor = 0;
	delay = (long)(fdelay * (float)(size - 1));
}


void Gijzelijzer::setProgramName (char *name)
{
	strcpy (programs[curProgram].name, name);
}


void Gijzelijzer::getProgramName (char *name)
{
	if (!strcmp (programs[curProgram].name, "Init"))
		sprintf (name, "%s %d", programs[curProgram].name, curProgram + 1);
	else
		strcpy (name, programs[curProgram].name);
}

bool Gijzelijzer::getProgramNameIndexed (VstInt32 category, VstInt32 index, char* text)
{
	if (index < kNumPrograms)
	{
		strcpy (text, programs[index].name);
		return true;
	}
	return false;
}


void Gijzelijzer::resume ()
{
	memset (buffer, 0, size * sizeof (float));
	AudioEffectX::resume ();
}


void Gijzelijzer::setParameter (VstInt32 index, float value)
{
    GijzelijzerProgram* ap = &programs[curProgram];

	switch (index)
	{
		case kDelay :    setDelay (value);					break;
		case kFeedBack : fFeedBack = ap->fFeedBack = value; break;
		case kOut :      fOut = ap->fOut = value;			break;
	}
}


float Gijzelijzer::getParameter (VstInt32 index)
{
	float v = 0;

	switch (index)
	{
		case kDelay :    v = fDelay;	break;
		case kFeedBack : v = fFeedBack; break;
		case kOut :      v = fOut;		break;
	}
	return v;
}


void Gijzelijzer::getParameterName (VstInt32 index, char *label)
{
	switch (index)
	{
		case kDelay :    strcpy (label, "Delay");		break;
		case kFeedBack : strcpy (label, "FeedBack");	break;
		case kOut :      strcpy (label, "Volume");		break;
	}
}


void Gijzelijzer::getParameterDisplay (VstInt32 index, char *text)
{
	switch (index)
	{
		case kDelay :    int2string (delay, text, kVstMaxParamStrLen);			break;
		case kFeedBack : float2string (fFeedBack, text, kVstMaxParamStrLen);	break;
		case kOut :      dB2string (fOut, text, kVstMaxParamStrLen);			break;
	}
}


void Gijzelijzer::getParameterLabel (VstInt32 index, char *label)
{
	switch (index)
	{
		case kDelay :    strcpy (label, "samples");	break;
		case kFeedBack : strcpy (label, "amount");	break;
		case kOut :      strcpy (label, "dB");		break;
	}
}


bool Gijzelijzer::getEffectName (char* name)
{
    strcpy (name, "Reverser");
	return true;
}


bool Gijzelijzer::getProductString (char* text)
{
    strcpy (text, "Reverser");
	return true;
}


bool Gijzelijzer::getVendorString (char* text)
{
    strcpy (text, "Pythonic.nl");
	return true;
}


void Gijzelijzer::processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames)
{
	float* in = inputs[0];
	float* out1 = outputs[0];
	float* out2 = outputs[1];
	float x, y;
	VstInt32 current = sampleFrames;

	while (--current >= 0) {
		x = *in++;
		y = buffer[cursor];
        buffer[cursor++] = x + y * fFeedBack;
		if (cursor >= delay) {
			cursor = 0;
		}
		*out1++ = y;
		if (out2) {
			*out2++ = y;
		}
	}
}
