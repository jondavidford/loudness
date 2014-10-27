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

#include "StereoToMono.h"

namespace loudness{

    StereoToMono::StereoToMono() : Module("StereoToMono") {};

    StereoToMono::~StereoToMono() : {};

    bool StereoToMono::initializeInternal(const TrackBank &input)
    {
        if (input.getNTracks() % 2 == 0 ) 
        { 
            //initialize output TrackBank
            output_.initialize(input.getNTracks() / 2, input.getNChannels(), input.getNSamples(), input.getFs());

            return 1;
        }
        else
        {
            LOUDNESS_ERROR(name_ 
                << ": TrackBank must have an even number of tracks")
            return 0;
        }
    }

    void StereoToMono::processInternal(const TrackBank &input)
    {
        int nTracks;
        Real l,r, avg;

        nTracks = input.getNTracks();

        for (int track = 0; track < nTracks; track += 2)
        {
            for (chn=0; chn < input.getNChannels(); chn++)
            {
                //input sample
                r = input.getSample(track, chn, 0);
                l = input.getSample(track+1, chn, 0);

                // take the average of the magnitude spectrum 
                avg = pow(sqrt(r) + sqrt(l) / 2, 2);

                //output sample
                output_.setSample((int) track / 2, chn, 0, avg);
            }
        }
    }

    void StereoToMono::resetInternal()
    {
    }

}
