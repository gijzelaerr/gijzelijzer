
#ifndef __gijzelijzer__
#include "gijzelijzer.h"
#endif


AudioEffect* createEffectInstance (audioMasterCallback audioMaster)
{
    return new Gijzelijzer (audioMaster);
}

