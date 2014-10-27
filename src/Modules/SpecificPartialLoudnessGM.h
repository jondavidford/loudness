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

#ifndef SPECIFICPARTIALLOUDNESSGM_H
#define SPECIFICPARTIALLOUDNESSGM_H

#include "../Support/Module.h"

/*
 * =====================================================================================
 *        Class:  
 *  Description:   This represents the loudness per ERB. 
 *
 *                TODO(dominic.ward@bcu.ac.uk):  Allow for high level excitation to
 *                                               specific loudness mapping according to
 *                                               ANSI S3.4-2007.
 * =====================================================================================
 */

namespace loudness{

    /**
     * @class PartialLoudnessGM
     * @brief Applies piecewise function to input target and noise excitation 
     * patterns (in linear power units) and outputs a corresponding specific 
     * loudness pattern for the target in noise.
     * 
     * Only for use within DynamicPartialLoudnessGM model.
     *
     * Models the compressive behaviour of the active cochlea mechanism
     * according to:
     * 
     * Moore, B. C. J., Glasberg, B. R., & Baer, T. (1997).  A Model for the
     * Prediction of Thresholds, Loudness, and Partial Loudness.  Journal of the
     * Audio Engineering Society, 45(4), 224–240.
     *
     * ANSI S3.4-2007.  (2007).  Procedure for the Computation of Loudness of
     * Steady Sounds.
     */
    class SpecificPartialLoudnessGM : public Module
    {
    public:

        SpecificPartialLoudnessGM(bool ansiS3407=0);

        virtual ~SpecificPartialLoudnessGM();

    private:

        virtual bool initializeInternal(const TrackBank &input);

        virtual void processInternal(const TrackBank &input);

        virtual void resetInternal();

        bool ansiS3407_;
        int nFiltersLT500_;
        Real cParam_, c2Param;
        RealVec eThrqParam_, eThrnParam_, gParam_, aParam_, alphaParam_, k_;
    };
}

#endif
