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

#ifndef  IIR_H
#define  IIR_H

#include "../Support/FilterBank.h"

/*
 * =====================================================================================
 *        Class:  IIR
 *  Description:  Performs IIR filtering of an input TrackBank using direct form 2.
 *
 * =====================================================================================
 */

namespace loudness{

    class IIR : public Module, public FilterBank
    {
        public:
            /*
            *--------------------------------------------------------------------------------------
            *       Class:  IIR
            *      Method:  IIR :: IIR
            * Description:  Constructs an IIR object.
            *  Parameters:  bCoefs:  Filter coefficients.
            *--------------------------------------------------------------------------------------
            */
            IIR(int nTracks, const RealVec &bCoefs, const RealVec &aCoefs);
            IIR();

            virtual ~IIR();
            /*
            *--------------------------------------------------------------------------------------
            *       Class:  IIR
            *      Method:  IIR :: initializeInternal
            * Description:  Sets the filter order, initializes the delay line and 
            *               configures the output TrackBank.  
            *  Parameters:  input:  The TrackBank to be filtered.
            *--------------------------------------------------------------------------------------
            */
        private:

            virtual bool initializeInternal(const TrackBank &input);
            /*
            *--------------------------------------------------------------------------------------
            *       Class:  IIR
            *      Method:  IIR :: ProcessInternal
            * Description:  Performs IIR filtering of the input TrackBank.
            *               Filtered data is stored in the output TrackBank.
            *  Parameters:  input:  The TrackBank to be filtered.
            *--------------------------------------------------------------------------------------
            */
            virtual void processInternal(const TrackBank &input);
            virtual void resetInternal();
            virtual void resizeInternal(int nTracks);
    };
}

#endif

