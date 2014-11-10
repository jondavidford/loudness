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

    StereoToMono::~StereoToMono() {};

    bool StereoToMono::initializeInternal(const TrackBank &input)
    {
        if (input.getNTracks() % 2 == 0 ) 
        { 
            //initialize output TrackBank
            output_.initialize(input.getNTracks() / 2, input.getNChannels(), input.getNSamples(), input.getFs());
            output_.setCentreFreqs(input.getCentreFreqs());

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
        Real l,r, pos;

        nTracks = input.getNTracks();

        for (int target = 0; target < nTracks; target += 2)
        {
            for (int chn = 0; chn < input.getNChannels(); chn++)
            {

                //input sample
                r = sqrt(input.getSample(target, chn, 0));
                l = sqrt(input.getSample(target+1, chn, 0));

                // calculate spatial position
                if (l == 0) // signal all the way left
                {
                    pos = -90.0;
                }
                else if  (r == 0) // signal all the way right
                {
                    pos = 90.0;
                }
                else if (l > r) // signal partially left
                {
                    pos = (r / l) * -90.0;
                }
                else // signal partially right
                {
                    pos = (l / r) * 90.0;
                }

                output_.setSpatialPosition((int) target / 2, chn, pos);

                // set output to average of magnitude spectrum
                output_.setSample((int) target / 2, chn, 0, pow((r + l) / 2, 2));
            }
        }
    }

    void StereoToMono::resetInternal()
    {
    }

}
