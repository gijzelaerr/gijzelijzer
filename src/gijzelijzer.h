
#ifndef __gijzelijzer__
#define __gijzelijzer__

#include "public.sdk/source/vst2.x/audioeffectx.h"

enum
{
	// Global
	kNumPrograms = 16,

	// Parameters Tags
	kDelay = 0,
	kFeedBack,
	kOut,

	kNumParams
};

class Gijzelijzer;


class GijzelijzerProgram
{
friend class Gijzelijzer;
public:
    GijzelijzerProgram ();
    ~GijzelijzerProgram () {}

private:	
	float fDelay;
	float fFeedBack;
	float tempo;
	float fOut;
	char name[24];
};


class Gijzelijzer : public AudioEffectX
{
public:
    Gijzelijzer (audioMasterCallback audioMaster);
    ~Gijzelijzer ();

    virtual void processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames);

	virtual void setProgram (VstInt32 program);
	virtual void setProgramName (char* name);
	virtual void getProgramName (char* name);
	virtual bool getProgramNameIndexed (VstInt32 category, VstInt32 index, char* text);
	
	virtual void setParameter (VstInt32 index, float value);
	virtual float getParameter (VstInt32 index);
	virtual void getParameterLabel (VstInt32 index, char* label);
	virtual void getParameterDisplay (VstInt32 index, char* text);
	virtual void getParameterName (VstInt32 index, char* text);

	virtual void resume ();

	virtual bool getEffectName (char* name);
	virtual bool getVendorString (char* text);
	virtual bool getProductString (char* text);
	virtual VstInt32 getVendorVersion () { return 1000; }
	
	virtual VstPlugCategory getPlugCategory () { return kPlugCategEffect; }

protected:
	void setDelay (float delay);

    GijzelijzerProgram* programs;
	
	float* buffer;
	float fDelay;
	float fFeedBack;
	float fOut;
	float fTempo;
	
	long delay;
	long size;
	long cursor;
};

#endif
