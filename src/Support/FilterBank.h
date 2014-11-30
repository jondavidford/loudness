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

#ifndef FILTERBANK_H
#define FILTERBANK_H

#include "../Support/Module.h"

/*
 * =====================================================================================
 *        Class:  Filter
 *  Description:  Base class for other filter classes.
 * =====================================================================================
 */

namespace loudness{

    /**
     * @class FilterBank
     *
     * @brief Abstract class for other FilterBank subclasses.
     *
     * This class provides some general functions common to digital FilterBanks 
     *
     * @author Dominic Ward
     *
     * @sa FIR, IIR, Butter
     */
    class FilterBank
    {
    public:
        FilterBank();
        FilterBank(int order);
        virtual ~FilterBank();

        /**
         * @brief Loads feedforward (@a bCoefs) and/or feedback (@a aCofs)
         * FilterBank coefficients from a Numpy array binary file.
         * 
         * The shape of the array should be (1xN) and (2xN) for FIR and IIR
         * FilterBanks respectively, where N is the FilterBank order plus one.
         *
         * I have not implemented this method as a constructor as the loading of
         * a Numpy array can fail.
         *
         * @param pathToFilterBankCoefs A string representing the path to the Numpy
         * array binary file. The file name must end with the
         * .npy extension.
         *
         * @return true if successfully loaded, false otherwise.
         */
        bool loadCoefsFromNumpyArray(string pathToFilterBankCoefs="");

        /**
         * @brief Sets the number of tracks (i.e. the number of filters)
         */
        void setNTracks(const int nTracks);

        /**
         * @brief Sets the feedforward coefficients.
         */
        void setBCoefs(const RealVec &bCoefs);

        /**
         * @brief Sets the feedback coefficients.
         */
        void setACoefs(const RealVec &aCoefs);

        /**
         * @brief Normalises the FilterBank coefficients by the first feedback
         * coefficient (aCoefs[0]);
         */
        void normaliseCoefs();
         
        /**
         * @brief Sets the linear gain of the digital FilterBank.
         */
        void setGain(Real gain);

        /**
         * @brief Returns the linear gain of the digital FilterBank.
         */
        Real getGain() const;

        /**
         * @brief Returns the order of the digital FilterBank.
         *
         * This is equal to the maximum number of feedforward
         * or feedback coefficients minus one.
         */
        int getOrder() const;

        /**
         * @brief Clears the internal state of the FilterBank.
         */
        void resetDelayLine();

    protected:
        Real gain_;
        int order_, orderMinus1_, nTracks_;
        RealVec bCoefs_, aCoefs_;
        RealVecVec z_;
    };
}

#endif
