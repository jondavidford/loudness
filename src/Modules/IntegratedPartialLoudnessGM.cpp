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

#include "IntegratedPartialLoudnessGM.h"
#include "../Support/AuditoryTools.h"

namespace loudness{

    IntegratedPartialLoudnessGM::IntegratedPartialLoudnessGM(bool diotic, bool uniform, Real cParam) :
        diotic_(diotic),
        uniform_(uniform),
        cParam_(cParam)
    {
        LOUDNESS_DEBUG("IntegratedLoudness: Constructed.");
        name_ = "IntegratedLoudness";
    }

    IntegratedPartialLoudnessGM::~IntegratedPartialLoudnessGM()
    {
    };

    void IntegratedPartialLoudnessGM::setAttackSTLCoef(Real tau)
    {
        LOUDNESS_DEBUG("IntegratedPartialLoudnessGM: STL attack time: " << tau);
        attackSTLCoef_ = 1-exp(-timeStep_/tau);
    }

    void IntegratedPartialLoudnessGM::setReleaseSTLCoef(Real tau)
    {
        LOUDNESS_DEBUG("IntegratedPartialLoudnessGM: STL release time: " << tau);
        releaseSTLCoef_ = 1-exp(-timeStep_/tau);
    }

    void IntegratedPartialLoudnessGM::setAttackLTLCoef(Real tau)
    {
        LOUDNESS_DEBUG("IntegratedPartialLoudnessGM: LTL attack time: " << tau);
        attackLTLCoef_ = 1-exp(-timeStep_/tau);
    }

    void IntegratedPartialLoudnessGM::setReleaseLTLCoef(Real tau)
    {
        LOUDNESS_DEBUG("IntegratedPartialLoudnessGM: LTL release time: " << tau);
        releaseLTLCoef_ = 1-exp(-timeStep_/tau);
    }

    bool IntegratedPartialLoudnessGM::initializeInternal(const TrackBank &input)
    {
        if(input.getNChannels()<1)
        {
            LOUDNESS_ERROR("IntegratedPartialLoudnessGM: Insufficient channels.");
            return 0;
        }

        //assumes uniformly spaced ERB filters
        camStep_ = FreqToCam(input.getCentreFreq(1))-FreqToCam(input.getCentreFreq(0));
        LOUDNESS_DEBUG("IntegratedPartialLoudnessGM: Filter spacing (Cams): " << camStep_);

        //if excitation pattern is sampled non-uniformly, approximate integral
        if(!uniform_)
        {
            for(int i=1; i<input.getNChannels(); i++)
            {
                //compute difference between subsequent filters on cam scale
                Real step = FreqToCam(input.getCentreFreq(i)) -
                    FreqToCam(input.getCentreFreq(i-1));
                //store
                camDif_.push_back(step);
            }

            //overwrite camStep_
            camStep_ = 1;
        }

        //if diotic presentation, multiply total loudness by 2
        if(diotic_)
        {
            cParam_ *= 2;
            LOUDNESS_DEBUG("IntegratedPartialLoudnessGM: Diotic presentation, loudness will be multiplied by 2.");
        }

        //update scaling factor
        cParam_ *= camStep_;

        //coefficient configuration
        timeStep_ = 1.0/input.getFrameRate();
        LOUDNESS_DEBUG("IntegratedPartialLoudnessGM: Time step: " << timeStep_);
        
        //short-term loudness time-constants (alpha from paper)
        setAttackSTLCoef(-0.001/log(1-0.045));    
        setReleaseSTLCoef(-0.001/log(1-0.02));

        //long-term loudness time-constants
        setAttackLTLCoef(-0.001/log(1-0.01));
        setReleaseLTLCoef(-0.001/log(1-0.0005));

        int nTracks = input.getNTracks();
        prevSTL_.assign(nTracks, 0);
        prevLTL_.assign(nTracks, 0);
        prevSTPL_.assign(nTracks, 0);
        prevLTPL_.assign(nTracks, 0);

        //output TrackBank
        output_.initialize(nTracks, input.getNChannels(), 6, input.getFs());
        output_.setFrameRate(input.getFrameRate());

        return 1;
    }

    void IntegratedPartialLoudnessGM::processInternal(const TrackBank &input)
    {       
        Real il, stl, ltl, ilChn, stlChn, ltlChn, prevSTLChn, prevLTLChn;
        Real ipl, stpl, ltpl, iplChn, stplChn, ltplChn, prevSTPLChn, prevLTPLChn;
        for (int track = 0; track < input.getNTracks(); track++)
        {
            // first get overall loudnesses
            // in order to calculate if sound is in attack or release phase
            //
            // loudness
            il = 0;
            if(uniform_)
            {
                for(int chn=0; chn<input.getNChannels(); chn++)
                    il += input.getSample(track, chn, 0);
            }
            else
            {
                for(int chn=0; chn<input.getNChannels()-1; chn++)
                {
                    il += input.getSample(track, chn, 0)*camDif_[chn] + 0.5*camDif_[chn]*
                        (input.getSample(track, chn+1, 0)-input.getSample(track, chn, 0));
                }
            }

            //apply scaling factor
            il *= cParam_;

            //short-term loudness
            Real prevSTL = prevSTL_[track];

            if(il>prevSTL)
                stl = attackSTLCoef_*(il-prevSTL) + prevSTL;
            else
                stl = releaseSTLCoef_*(il-prevSTL) + prevSTL;

            //long-term loudness
            Real prevLTL = prevLTL_[track];
            if(stl>prevLTL)
                ltl = attackLTLCoef_*(stl-prevLTL) + prevLTL;
            else
                ltl = releaseLTLCoef_*(stl-prevLTL) + prevLTL;

            // first get overall loudnesses
            // in order to calculate if sound is in attack or release phase
            ipl = 0;
            if(uniform_)
            {
                for(int chn=0; chn<input.getNChannels(); chn++)
                    ipl += input.getSample(track, chn, 1);
            }
            else
            {
                for(int chn=0; chn<input.getNChannels()-1; chn++)
                {
                    ipl += input.getSample(track, chn, 1)*camDif_[chn] + 0.5*camDif_[chn]*
                        (input.getSample(track, chn+1, 1)-input.getSample(track, chn, 1));
                }
            }

            //apply scaling factor
            ipl *= cParam_;

            //short-term loudness
            Real prevSTPL = prevSTPL_[track];

            if(il>prevSTPL)
                stpl = attackSTLCoef_*(ipl-prevSTPL) + prevSTPL;
            else
                stpl = releaseSTLCoef_*(ipl-prevSTPL) + prevSTPL;

            //long-term loudness
            Real prevLTPL = prevLTPL_[track];
            if(stl>prevLTPL)
                ltpl = attackLTLCoef_*(stpl-prevLTPL) + prevLTPL;
            else
                ltpl = releaseLTLCoef_*(stpl-prevLTPL) + prevLTPL;
            
            // then get individual channel loudnesses
            // using info about if the sound is in attack or release phase
            for (int chn = 0; chn < input.getNChannels(); chn++)
            {
                ilChn = input.getSample(track, chn, 0);
                iplChn = input.getSample(track, chn, 1);

                prevSTLChn = output_.getSample(track, chn, 1);
                prevLTLChn = output_.getSample(track, chn, 2);
                prevSTPLChn = output_.getSample(track, chn, 4);
                prevLTPLChn = output_.getSample(track, chn, 5);

                if(il>prevSTL)
                    stlChn = attackSTLCoef_*(ilChn-prevSTLChn) + prevSTLChn;
                else
                    stlChn = releaseSTLCoef_*(ilChn-prevSTLChn) + prevSTLChn;

                if(stl>prevLTL)
                    ltlChn = attackLTLCoef_*(stl-prevLTLChn) + prevLTLChn;
                else
                    ltlChn = releaseLTLCoef_*(stl-prevLTLChn) + prevLTLChn;

                if(ipl>prevSTPL)
                    stplChn = attackSTLCoef_*(iplChn-prevSTPLChn) + prevSTPLChn;
                else
                    stplChn = releaseSTLCoef_*(iplChn-prevSTPLChn) + prevSTPLChn;

                if(stpl>prevLTPL)
                    ltplChn = attackLTLCoef_*(stpl-prevLTPLChn) + prevLTPLChn;
                else
                    ltplChn = releaseLTLCoef_*(stpl-prevLTPLChn) + prevLTPLChn;

                output_.setSample(track, chn, 0, ilChn);
                output_.setSample(track, chn, 1, stlChn);
                output_.setSample(track, chn, 2, ltlChn);
                output_.setSample(track, chn, 3, iplChn);
                output_.setSample(track, chn, 4, stplChn);
                output_.setSample(track, chn, 5, ltplChn);
            }

            prevSTL_[track] = stl;
            prevLTL_[track] = ltl;
            prevSTPL_[track] = stpl;
            prevLTPL_[track] = ltpl;
        }
    }

    //output TrackBanks are cleared so not to worry about filter state
    void IntegratedPartialLoudnessGM::resetInternal()
    {
    }
}

