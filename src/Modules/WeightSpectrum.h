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

#ifndef WEIGHTSPECTRUM_H
#define WEIGHTSPECTRUM_H

#include "../Support/Module.h"
#include "../Support/AuditoryTools.h"

namespace loudness{

    /**
     * @class TrackBank
     * @brief Filters an input TrackBank by applying a set of weights.
     *
     * This algorithm scales an input TrackBank using the set of weights. The
     * weights should be specified in decibels. As the name implies, this module
     * is used for modifying a short-time power spectrum, such as the kind
     * output by PowerSpectrum.
     *
     * Note: Use of OME class internally is a hacky.
     *
     * @author Dominic Ward
     */
    class WeightSpectrum : public Module
    {

    public:

        /**
         * @brief Constructs the module using a set of input weights.
         *
         * @param vector of weights (in decibels).
         */
        WeightSpectrum(const RealVec &weights);

        /**
         * @brief Constructs the module for use with class OME.
         *
         * @param middleEarType The middle ear type.
         * @param outerEarType The outer ear type.
         */
        WeightSpectrum(OME::MiddleEarType middleEarType = OME::ANSI,
                OME::OuterEarType outerEarType = OME::ANSI_FREE);

        virtual ~WeightSpectrum();

        /**
         * @brief Set the vector of weights (in decibels).
         */
        void setWeights(const RealVec &weights);

    private:
        virtual bool initializeInternal(const TrackBank &input);

        virtual void processInternal(const TrackBank &input);

        virtual void resetInternal();

        RealVec weights_;
        OME ome_;
        bool usingOME_;
    };
}

#endif
