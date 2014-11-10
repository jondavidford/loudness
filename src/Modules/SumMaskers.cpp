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

#include "SumMaskers.h"
#include "../Support/AuditoryTools.h"

namespace loudness{

    SumMaskers::SumMaskers() : Module("SumMaskers") {};

    SumMaskers::~SumMaskers() {};

    bool SumMaskers::initializeInternal(const TrackBank &input)
    {
        //initialize output TrackBank
        output_.initialize(input.getNTracks() * 2, input.getNChannels(), input.getNSamples(), input.getFs());
        output_.setCentreFreqs(input.getCentreFreqs());

        avgPositions_.resize(input.getNTracks());

        return 1;
    }

    void SumMaskers::processInternal(const TrackBank &input)
    {
        int nTracks, nChannels, sumIndex;
        Real sum, separation, reduction;

        nTracks = input.getNTracks();
        nChannels = input.getNChannels();

        for (int track = 0; track < nTracks; track++)
        {
            sum = 0;
            for (int chn = 0; chn < nChannels; chn++)
            {
                sum += input.getSpatialPosition(track, chn);
            }

            // average position of track
            avgPositions_[track] = sum / nChannels;
        }

        for (int target = 0; target < nTracks; target++)
        {
            // set output to input
            // initialize masker track with 0 intensity
            sumIndex = target + nTracks;
            for (int chn = 0; chn < nChannels; chn++)
            {
                output_.setSample(target, chn, 0, input.getSample(target, chn, 0));
                output_.setSample(sumIndex, chn, 0, 0);
            }
            
            for (int masker = 0; masker < nTracks; masker++)
            {
                if (target != masker)
                {
                    separation = abs(avgPositions_[target] - avgPositions_[masker]);
                    reduction = separationToReduction(separation);
                    for (int chn = 0; chn < nChannels; chn++)
                        output_.sumSample(sumIndex, chn, 0, input.getSample(masker, chn, 0) * reduction);
                }
            }
        }
    }

    void SumMaskers::resetInternal()
    {
    }

}