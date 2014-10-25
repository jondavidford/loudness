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

#ifndef DOUBLEROEXBANK_H
#define DOUBLEROEXBANK_H

#include "../Support/Module.h"

namespace loudness{

    //Need to run tests with this class
    class DoubleRoexBank : public Module
    {

    public:

        DoubleRoexBank(Real camLo = 1.5, Real camHi = 40.1, Real camStep = 0.1);

        virtual ~DoubleRoexBank();

    private:

        virtual bool initializeInternal(const TrackBank &input);

        virtual void processInternal(const TrackBank &input);

        virtual void resetInternal();

        int nFilters_;
        Real camLo_, camHi_, camStep_;
        RealVec maxGdB_, thirdGainTerm_;
        RealVecVec wPassive_, wActive_;
    };
}

#endif
