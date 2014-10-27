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

#ifndef  STEREOTOMONO_H
#define  STEREOTOMONO_H

#include "../Support/Module.h"

/*
 * =====================================================================================
 *        Class:  StereoToMono
 *  Description:  Converts a stereo signal into mono. Input is a power spectrum and
 *                output is a power spectrum. Assumes that a single stereo track is
 *                adjacent tracks in the input TrackBank (i.e. left channel of track
 *                1 is index 0 and right channel is index 1)
 *
 * =====================================================================================
 */

namespace loudness{

    class StereoToMono : public Module
    {
        public:

            StereoToMono(const RealVec &bCoefs, const RealVec &aCoefs);
            StereoToMono();

            virtual ~StereoToMono();

        private:

            virtual bool initializeInternal(const TrackBank &input);
            virtual void processInternal(const TrackBank &input);
            virtual void resetInternal();
    };
}

#endif

