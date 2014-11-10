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

#ifndef  SUMMASKERS_H
#define  SUMMASKERS_H

#include "../Support/Module.h"

/*
 * =====================================================================================
 *        Class:  SumMaskers
 *  Description:  Creates nTracks new tracks, each the sum of all signals other than the 
 *                target. Also takes into account spatial separation data between tracks
 *                and reduces input signals accordingly.
 *                Each sum is at index target + nTracks
 *
 * =====================================================================================
 */

namespace loudness{

    class SumMaskers : public Module
    {
        public:
            SumMaskers();
            virtual ~SumMaskers();

        private:

            virtual bool initializeInternal(const TrackBank &input);
            virtual void processInternal(const TrackBank &input);
            virtual void resetInternal();

            RealVec avgPositions_;
    };
}

#endif