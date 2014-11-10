/*
 * Copyright (C) 2014 Dominic Ward <contactdominicward@gmail.com>
 *
 * This file is part of Loudness
 *
 * Loudness is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Loudness is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Loudness.  If not, see <http://www.gnu.org/licenses/>. 
 */

#include "SpecificPartialLoudnessGM.h"
#include "../Support/AuditoryTools.h"

namespace loudness{

    SpecificPartialLoudnessGM::SpecificPartialLoudnessGM(bool ansiS3407) :
        Module("SpecificPartialLoudnessGM"),
        ansiS3407_(ansiS3407)
    {}

    SpecificPartialLoudnessGM::~SpecificPartialLoudnessGM() {}

    bool SpecificPartialLoudnessGM::initializeInternal(const TrackBank &input)
    {
        /*
        if (input.getNTracks() != 2)
        {
            LOUDNESS_ERROR(name_ << ": TrackBank must have exactly 2 tracks");
        }*/
        //c value from ANSI 2007
        cParam_ = 0.046871;
        c2Param_ = 0.0000459608; // C_2 = C / (1.04 x 10^6)^0.5

        Real eThrqdB500Hz = IntExc(500);

        //Number of filters below 500Hz
        nFiltersLT500_ = 0;

        //fill loudness parameter vectors
        Real fcPrev = input.getCentreFreq(0);
        for(int i=0; i<input.getNChannels(); i++)
        {
            Real fc = input.getCentreFreq(i);

            if(fc<fcPrev)
                LOUDNESS_ERROR("SpecificPartialLoudnessGM: Centre frequencies are not in ascending order!");
            fcPrev = fc;

            Real eThrqdB = IntExc(fc);
            if (fc < 500)
            {
                eThrqParam_.push_back(pow(10,eThrqdB/10.0));
                nFiltersLT500_++;
            }
            else
            {
                eThrqParam_.push_back(pow(10,eThrqdB500Hz/10));
            }

            Real gdB = eThrqdB500Hz-eThrqdB;
            k_.push_back(pow(10, kdB(fc)/10));
            gParam_.push_back(pow(10, gdB/10.0));
            aParam_.push_back(GdBToA(gdB));
            alphaParam_.push_back(GdBToAlpha(gdB));
        }
        LOUDNESS_DEBUG("SpecificPartialLoudnessGM: number of filters <500 Hz: " << nFiltersLT500_);

        //output TrackBank
        output_.initialize(1, input.getNChannels(), input.getNSamples(), input.getFs());

        return 1;
    }

    void SpecificPartialLoudnessGM::processInternal(const TrackBank &input)
    {
        Real eSig, eNoise, eThrn, sl=0.0;

        for(int freq = 0; freq < input.getNChannels(); freq++)
        {
            eSig = input.getSample(0, freq, 0);
            /*
            eNoise = input.getSample(1, freq, 0);
            eThrn = k_[freq] * eNoise + eThrqParam_[freq];

            // partial loudness calculation (the target signal in noise)
            if (eSig + eNoise > 1e10)
            {
                if (eSig >= eThrqParam_[freq]) // equation 19 from Glasberg and Moore 1997
                {
                    sl = c2Param_ * pow(eSig + eNoise, 0.5)
                            - c2Param_ * (pow((1 + k_[freq]) * eNoise + eThrqParam_[freq], 0.5)
                                            - pow(eThrqParam_[freq] * gParam_[freq] + aParam_[freq], alphaParam_[freq])
                                            + pow(aParam_[freq], alphaParam_[freq]))
                                        * pow(eThrqParam_[freq] / eSig, 0.3);
                }
                else // equation 20 from Glasberg and Moore 1997
                {
                    sl = cParam_
                        * pow(2 * eSig / (eSig + eThrqParam_[freq]), 1.5)
                        * ((pow(eThrqParam_[freq] * gParam_[freq] + aParam_[freq], alphaParam_[freq])
                                - pow(aParam_[freq], alphaParam_[freq]))
                            / (pow(eNoise * (1 + k_[freq]) + eThrqParam_[freq], 0.5) - pow(eNoise, 0.5)))
                        * (pow(eSig + eNoise, 0.5) - pow(eNoise, 0.5));
                }
            }
            else // eSig + eNoise <= 1e10
            {
                if (eSig >= eThrn) // // equation 17 from Glasberg and Moore 1997
                {
                    sl = cParam_ * (pow((eSig + eNoise) * gParam_[freq] + aParam_[freq], alphaParam_[freq])
                                    - pow(aParam_[freq], alphaParam_[freq]))
                        - cParam_
                            * (pow((eNoise * (1 + k_[freq]) + eThrqParam_[freq]) * gParam_[freq] + aParam_[freq], alphaParam_[freq])
                                - pow(eThrqParam_[freq] * gParam_[freq] + aParam_[freq], alphaParam_[freq]))
                            * pow(eThrn / eSig, 0.3);
                }
                else // equation 18 from Glasberg and Moore 1997
                {
                    sl = cParam_
                        * pow(2 * eSig / (eSig + eThrqParam_[freq]), 1.5)
                        * ((pow(eThrqParam_[freq] * gParam_[freq] + aParam_[freq], alphaParam_[freq])
                                - pow(aParam_[freq], alphaParam_[freq]))
                            / (pow(eNoise * (1 + k_[freq]) + eThrqParam_[freq], 0.5) - pow(eNoise, 0.5)));
                }
            }

            // set partial loudness at output channel 1
            output_.setSample(0, freq, 1, 10*log10(sl));
            */

            // loudness calculation (the isolated target signal)
            //high level
            if (eSig > 1e10)
            {
                if(ansiS3407_)
                    sl = pow((eSig/1.0707),0.2);
                else
                    sl = pow((eSig/1.04e6),0.5);
            }
            else if(freq<nFiltersLT500_) //low freqs
            { 
                if(eSig>eThrqParam_[freq]) //medium level
                {
                    sl = (pow(gParam_[freq]*eSig+aParam_[freq], alphaParam_[freq])-
                            pow(aParam_[freq],alphaParam_[freq]));
                }
                else //low level
                {
                    sl = pow((2*eSig)/(eSig+eThrqParam_[freq]), 1.5)*
                        (pow(gParam_[freq]*eSig+aParam_[freq],alphaParam_[freq])
                            -pow(aParam_[freq],alphaParam_[freq]));
                }
            }
            else //high freqs (variables are constant >= 500 Hz)
            { 
                if(eSig>2.3604782331805771) //medium level
                {
                    sl = pow(eSig+4.72096, 0.2)-1.3639739128330546;
                } 
                else //low level
                {
                    sl = pow((2*eSig)/(eSig+2.3604782331805771), 1.5)*
                        (pow(eSig+4.72096, 0.2)-1.3639739128330546);
                }
            }
            
            // set loudness at output channel 0
            output_.setSample(0, freq, 0, cParam_*sl);
        }
    }

    void SpecificPartialLoudnessGM::resetInternal(){};
}



