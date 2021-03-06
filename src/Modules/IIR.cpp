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

#include "IIR.h"

namespace loudness{

    IIR::IIR() : Module("IIR") {};

    IIR::IIR(int nTracks, const RealVec &bCoefs, const RealVec &aCoefs) :
        Module("IIR")
    {
        setNTracks(nTracks);
        setBCoefs(bCoefs);
        setACoefs(aCoefs);
    }

    IIR::~IIR()
    {
    }

    bool IIR::initializeInternal(const TrackBank &input)
    {
        LOUDNESS_WARNING(name_ 
                        << ": nTracks: " << input.getNTracks());
        //constants
        int n_b = (int)bCoefs_.size();
        int n_a = (int)aCoefs_.size();
        if(n_b*n_a > 0)
        {
            if(n_b > n_a)
            {
                aCoefs_.resize(n_b,0);
                order_ = n_b-1;
            }
            else
            {
                bCoefs_.resize(n_a,0);
                order_ = n_a-1;
            }
            LOUDNESS_DEBUG("IIR: Filter order is: " << order_);
            orderMinus1_ = order_-1;

            //Normalise coefficients if a[0] != 1
            normaliseCoefs();

            int nTracks = input.getNTracks();
            for (int i = 0; i < nTracks; i++)
                z_[i].assign(order_,0.0);

            //output TrackBank
            output_.initialize(input);

            return 1;
        }
        {
            LOUDNESS_ERROR("IIR: No filter coefficients");
            return 0;
        }
    }

    void IIR::processInternal(const TrackBank &input)
    {
        int smp, j;
        Real x,y;

        for(int track = 0; track < input.getNTracks(); track++)
        {
            for(smp=0; smp<input.getNSamples(); smp++)
            {
                //input sample
                x = input.getSample(track, 0, smp) * gain_;

                //output sample
                y = bCoefs_[0] * x + z_[track][0];
                output_.setSample(track, 0, smp, y);

                //fill delay
                for (j=1; j<order_; j++)
                    z_[track][j-1] = bCoefs_[j] * x + z_[track][j] - aCoefs_[j] * y;

                //final sample
                z_[track][orderMinus1_] = bCoefs_[order_] * x - aCoefs_[order_] * y;
            }
        }
    }

    void IIR::resetInternal()
    {
        resetDelayLine();
    }

    void IIR::resizeInternal(int nTracks)
    {
        setNTracks(nTracks);
        resetDelayLine();
    }

}


