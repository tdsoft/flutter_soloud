/*
SoLoud audio engine
Copyright (c) 2013-2020 Jari Komppa

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
   distribution.
*/

#include <stdio.h>
#include <string.h>
#include <android/log.h>
#include "soloud.h"
#include "soloud_eqfilter.h"

#define LOG_TAG "SoLoud"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

namespace SoLoud
{
    EqFilterInstance::EqFilterInstance(EqFilter *aParent)
    {
        LOGI("EqFilterInstance::EqFilterInstance - Initializing");
        mParent = aParent;
        initParams(11); // Updated to include 11 bands
        mParam[BAND1] = aParent->mVolume[BAND1 - BAND1];
        mParam[BAND2] = aParent->mVolume[BAND2 - BAND1];
        mParam[BAND3] = aParent->mVolume[BAND3 - BAND1];
        mParam[BAND4] = aParent->mVolume[BAND4 - BAND1];
        mParam[BAND5] = aParent->mVolume[BAND5 - BAND1];
        mParam[BAND6] = aParent->mVolume[BAND6 - BAND1];
        mParam[BAND7] = aParent->mVolume[BAND7 - BAND1];
        mParam[BAND8] = aParent->mVolume[BAND8 - BAND1];
        mParam[BAND9] = aParent->mVolume[BAND9 - BAND1];
        mParam[BAND10] = aParent->mVolume[BAND10 - BAND1];
        mParam[BAND11] = aParent->mVolume[BAND11 - BAND1];
        LOGI("EqFilterInstance::EqFilterInstance - Initialization complete");
    }

    /// Already defined in soloud.cpp:945
    float EqFilterInstance::catmullrom(float t, float p0, float p1, float p2, float p3)
    {
        LOGI("EqFilterInstance::catmullrom - t: %f, p0: %f, p1: %f, p2: %f, p3: %f", t, p0, p1, p2, p3);
        float result = 0.5f * (
                (2 * p1) +
                (-p0 + p2) * t +
                (2 * p0 - 5 * p1 + 4 * p2 - p3) * t * t +
                (-p0 + 3 * p1 - 3 * p2 + p3) * t * t * t
        );
        LOGI("EqFilterInstance::catmullrom - result: %f", result);
        return result;
    }

    void EqFilterInstance::fftFilterChannel(float *aFFTBuffer, unsigned int aSamples, float /*aSamplerate*/, time /*aTime*/, unsigned int /*aChannel*/, unsigned int /*aChannels*/)
    {
        LOGI("EqFilterInstance::fftFilterChannel - Start");
        comp2MagPhase(aFFTBuffer, aSamples / 2);
        unsigned int p;
        for (p = 0; p < aSamples / 2; p++)
        {
            int i = (int)floor(sqrt(p / (float)(aSamples / 2)) * (aSamples / 2));
            int p2 = (i / (aSamples / 16));
            int p1 = p2 - 1;
            int p0 = p1 - 1;
            int p3 = p2 + 1;
            if (p1 < 0) p1 = 0;
            if (p0 < 0) p0 = 0;
            if (p3 > 10) p3 = 10; // Updated to include 11 bands
            float v = (float)(i % (aSamples / 16)) / (float)(aSamples / 16);
            aFFTBuffer[p * 2] *= catmullrom(v, mParam[p0 + 1], mParam[p1 + 1], mParam[p2 + 1], mParam[p3 + 1]);
        }
        memset(aFFTBuffer + aSamples, 0, sizeof(float) * aSamples);
        magPhase2Comp(aFFTBuffer, aSamples / 2);
        LOGI("EqFilterInstance::fftFilterChannel - End");
    }

    result EqFilter::setParam(unsigned int aBand, float aVolume)
    {
        LOGI("EqFilter::setParam - aBand: %u, aVolume: %f", aBand, aVolume);
        if (aBand < BAND1 || aBand > BAND11) {
            LOGE("EqFilter::setParam - INVALID_PARAMETER");
            return INVALID_PARAMETER;
        }
        if (aVolume < getParamMin(BAND1) || aVolume > getParamMax(BAND1)) {
            LOGE("EqFilter::setParam - INVALID_PARAMETER");
            return INVALID_PARAMETER;
        }

        mVolume[aBand - BAND1] = aVolume;
        LOGI("EqFilter::setParam - SO_NO_ERROR");
        return SO_NO_ERROR;
    }

    int EqFilter::getParamCount()
    {
        LOGI("EqFilter::getParamCount - Return 12");
        return 12; // Updated to include 11 bands + 1 for "Wet"
    }

    const char* EqFilter::getParamName(unsigned int aParamIndex)
    {
        LOGI("EqFilter::getParamName - aParamIndex: %u", aParamIndex);
        switch (aParamIndex)
        {
            case BAND1: return "Band 1 (0.016 KHz)";
            case BAND2: return "Band 2 (0.0315 KHz)";
            case BAND3: return "Band 3 (0.063 KHz)";
            case BAND4: return "Band 4 (0.125 KHz)";
            case BAND5: return "Band 5 (0.25 KHz)";
            case BAND6: return "Band 6 (0.5 KHz)";
            case BAND7: return "Band 7 (1 KHz)";
            case BAND8: return "Band 8 (2 KHz)";
            case BAND9: return "Band 9 (4 KHz)";
            case BAND10: return "Band 10 (8 KHz)";
            case BAND11: return "Band 11 (16 KHz)";
        }
        return "Wet";
    }

    unsigned int EqFilter::getParamType(unsigned int aParamIndex)
    {
        LOGI("EqFilter::getParamType - aParamIndex: %u", aParamIndex);
        return FLOAT_PARAM;
    }

    float EqFilter::getParamMax(unsigned int aParamIndex)
    {
        LOGI("EqFilter::getParamMax - aParamIndex: %u", aParamIndex);
        if (aParamIndex == 0)
            return 1;
        return 4;
    }

    float EqFilter::getParamMin(unsigned int aParamIndex)
    {
        LOGI("EqFilter::getParamMin - aParamIndex: %u", aParamIndex);
        return 0;
    }

    EqFilter::EqFilter()
    {
        LOGI("EqFilter::EqFilter - Initializing");
        for (int i = 0; i < 11; i++)
            mVolume[i] = 1;
        LOGI("EqFilter::EqFilter - Initialization complete");
    }

    FilterInstance *EqFilter::createInstance()
    {
        LOGI("EqFilter::createInstance - Creating new EqFilterInstance");
        return new EqFilterInstance(this);
    }
}
