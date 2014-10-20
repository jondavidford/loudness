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

    bool SpecificPartialLoudnessGM::initializeInternal(const SignalBankBank &input)
    {
        //c value from ANSI 2007
        cParam_ = 0.046871;
        c2Param_ = 0.0000459608; // C_2 = C / (1.04 x 10^6)^0.5

        //Number of filters below 500Hz
        nFiltersLT500_ = 0;

        Real eThrqdB500Hz = IntExc(500);
        //fill loudness parameter vectors
        Real fcPrev = input.getCentreFreq(0);
        for(int i=0; i<input.getNChannels(); i++)
        {
            Real fc = input.getCentreFreq(i);

            if(fc<fcPrev)
                LOUDNESS_ERROR("SpecificPartialLoudnessGM: Centre frequencies are not in ascending order!");
            fcPrev = fc;

            if(fc<500)
            {
                Real eThrqdB = IntExc(fc);
                eThrqParam_.push_back(pow(10,eThrqdB/10.0));
                Real gdB = eThrqdB500Hz-eThrqdB;
                gParam_.push_back(pow(10, gdB/10.0));
                aParam_.push_back(GdBToA(gdB));
                alphaParam_.push_back(GdBToAlpha(gdB));
                /* 
                LOUDNESS_DEBUG("SpecificPartialLoudnessGM: eThrq: " <<
                        eThrqParam_[nFiltersLT500_] << ", gdB: " << gdB << ", A: "
                        << aParam_[nFiltersLT500_] << ", alpha: " <<
                        alphaParam_[nFiltersLT500_]);
                */
                nFiltersLT500_++;
            }
        }
        LOUDNESS_DEBUG("SpecificPartialLoudnessGM: number of filters <500 Hz: " << nFiltersLT500_);

        //output SignalBankBank
        output_.initialize(input);

        return 1;
    }

    void SpecificPartialLoudnessGM::processInternal(const SignalBankBank &input)
    {
        Real eSig, eNoise, sl=0.0;
        SignalBank masker;

        for(int track=0; track<input.getNTracks(); track++)
        {
            masker = input.sumSignalBanksExcept(track);
            for(int i=0; i<input.getSignalBank(track).getNChannels(); i++)
            {
                eSig = input.getSignalBank(track).getSample(i,0);
                eNoise = masker.getSample(i, 0);

                //high level
                if (eSig + eNoise > 1e10)
                {
                    if (eSig >= eThrqParam_[i]) // equation 19 from GM 1997
                    {
                        sl = c2Param_ * pow(eSig + eNoise, 0.5)
                                - c2Param * (pow((1 + kParam_) * eNoise + eThrqParam_[i], 0.5)
                                                - pow(eThrqParam_[i] * gParam[i] + aParam_[i], alphaParam_[i])
                                                + pow(aParam_[i], alphaParam_[i]))
                                            * pow(eThrqParam_[i] / eSig, 0.3);
                    }
                    else // equation 20 from GM 1997
                    {
                        sl = cParam
                            * pow(2 * eSig / (eSig + eThrqParam_[i]), 1.5)
                            * ((pow(eThrqParam_[i] * gParam_[i] + aParam_[i], alphaParam_[i])
                                    - pow(aParam_[i], alphaParam_[i]))
                                / (pow(eNoise * (1 + kParam_) + eThrqParam_[i], 0.5) - pow(eNoise, 0.5)))
                            * (pow(eSig + eNoise, 0.5) - pow(eNoise, 0.5));
                    }
                }
                else // eSig + eNoise <= 1e10
                {
                    if (eSig >= eThrnParam_[i])
                    {
                        sl = cParam * (pow((eSig + eNoise) * gParam_[i] + aParam_[i], alphaParam_[i])
                                        - pow(aParam_[i], alphaParam_[i]))
                            - cParam
                                * (pow((eNoise * (1 + kParam_) + eThrqParam_[i]) * gParam_[i] + aParam_[i], alphaParam_[i])
                                    - pow(eThrqParam_[i] * gParam_[i] + aParam_[i], alphaParam_[i]))
                                * pow(eThrnParam_[i] / eSig, 0.3);
                    }
                    else
                    {

                    }

                    if(excLin>eThrqParam_[i]) //medium level
                    {
                        sl = (pow(gParam_[i]*excLin+aParam_[i], alphaParam_[i])-
                                pow(aParam_[i],alphaParam_[i]));
                    }
                    else //low level
                    {
                        sl = pow((2*excLin)/(excLin+eThrqParam_[i]), 1.5)*
                            (pow(gParam_[i]*excLin+aParam_[i],alphaParam_[i])
                                -pow(aParam_[i],alphaParam_[i]));
                    }
                }
                else //high freqs (variables are constant >= 500 Hz)
                { 
                    if(excLin>2.3604782331805771) //medium level
                    {
                        sl = pow(excLin+4.72096, 0.2)-1.3639739128330546;
                    } 
                    else //low level
                    {
                        sl = pow((2*excLin)/(excLin+2.3604782331805771), 1.5)*
                            (pow(excLin+4.72096, 0.2)-1.3639739128330546);
                    }
                }
                
                output_.getSignalBank(track).setSample(i, 0, sl);
            }
        }
    }

    void SpecificPartialLoudnessGM::resetInternal(){};
}



