#pragma once

#include <stdint.h>

#ifndef __RANDOM__H__
inline uint32_t nLehmer;


inline uint32_t Lehmer32()
{
       nLehmer += 0xe120fc15;
       uint64_t tmp;
       tmp = (uint64_t) nLehmer*0x4a39b70d;
       uint32_t m1 = (tmp >> 32) ^ tmp;
       tmp = (uint64_t)m1 * 0x12fad5c9;
       uint32_t m2 = (tmp >> 32) ^ tmp;
       return m2;
}

inline void setLehmer(uint32_t num)
{
    nLehmer = num;
}
inline uint32_t getLehmer()
{
    return nLehmer;
}

inline int randint(int min, int max)
{
    return (Lehmer32() % ((max - min+1))) + min;
}

inline double randdouble(double min, double max)
{

        return ((double)Lehmer32() / (double)(0xFFFFFFFF) *(max - min)) + min;

}



   inline void perlInNoise2D(int nWidth,int nHeight, float *Seed, int nOctaves, float fBias, float *fOutput)
    {
        //Use 1D perlin noise
        for(int x = 0; x < nWidth; x++)
        {
            for(int y = 0; y < nHeight; y++)
            {

                float fNoise = 0.0f;
                float fScale = 1.0f;
                float fScaleAccum = 0.0f;
                for(int o = 0; o < nOctaves;o++)
                {

                    //Instead of nCount will be for our width because of our noise arrays always being powers of 2 and our pitching and sampling periods always wanting to be powers of 2 this is ok
                    

                    int nPitch = nWidth >> o; 
                    

                    //Instead of setting two sample points, will set sample points in the x and y axis
                    int sampleX1 = (x / nPitch) * nPitch;
                    int sampleY1 = (y / nPitch) * nPitch;

                    //and we will pick 2 samples because we will need to linearly interpolate between them

                    int sampleX2 = (sampleX1 + nPitch) % nWidth;
                    int sampleY2 = (sampleY1 + nPitch) % nWidth;


                    //Instead of a single blend we now need a blend in the x and y axis 
                    float fBlendX = (float)(x - sampleX1) / (float) nPitch;
                    float fBlendY = (float)(y - sampleY1) / (float) nPitch;


                    //And will pick 2 samples because we need to now linearly interpolate between them
                    
                    //We are effectively taking two slices of 1D perlin noise so we use fBlendX twice
                    float fSampleT = (1.0f - fBlendX) * Seed[sampleY1 * nWidth + sampleX1] + fBlendX * Seed[sampleY1 * nWidth + sampleX2];
                    float fSampleB = (1.0f - fBlendX) * Seed[sampleY2 * nWidth + sampleX1] + fBlendX * Seed[sampleY2 * nWidth + sampleX2];


                    //BlendY is used to linearly interpolate between the two samples
                    fNoise += (fBlendY * (fSampleB - fSampleT) + fSampleT) * fScale;

                    fScaleAccum += fScale;

                    fScale = fScale / fBias;
                    }


                fOutput[(y * nWidth) + x] = fNoise / fScaleAccum;
                //std::cout << fOutput[y * nWidth + x] << "\n";

                }
            }

        }

#endif