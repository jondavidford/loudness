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

#ifndef SIGNALBANKBANK_H
#define SIGNALBANKBANK_H

#include "Common.h"

namespace loudness{

    /**
     * @class SignalBankBank
     * 
     * @brief A bank of signals used as inputs and outputs of processing modules.
     *
     * Basically a wrapper for an array of SignalBanks. Used to stored meta data
     * about relationships between the different tracks (each SignalBank is a 
     * track)
     * 
     * @author Dominic Ward
     */
    class SignalBankBank
    {
    public:
        SignalBankBank();
        ~SignalBankBank();

        /**
         * @brief Initialises the SignalBankBank with input arguments.
         *
         * @param n_channels Number of channels.
         * @param n_samples Number of samples per channel.
         * @param fs Sampling frequency.
         */
        void initialize(int nTracks, int nChannels, int nSamples, int fs);

        /**
         * @brief Initialises the SignalBankBank with the same parameters as the 
         * input.
         *
         * @param input The input SignalBankBank whose structure will be copied.
         */
        void initialize(const SignalBankBank &input);

        /**
         * @brief Returns the number of tracks in the SignalBankBank
         *
         * @return Number of tracks
         */
        inline int getNTracks() const
        {
            return nTracks_;
        }
        
        /**
         * @brief Returns the number of channels in the SignalBank.
         *
         * @return Number of channels.
         */
        inline int getNChannels() const
        {
            return nChannels_;
        }

        /**
         * @brief Returns the number of samples per channel.
         *
         * @return Number of samples.
         */
        inline int getNSamples() const
        {
            return nSamples_;
        }

        /**
         * @brief Returns a single SignalBank.
         *
         * @param track Track index.
         *
         * @return SignalBank.
         */
        inline Real getSignalBank(int track) const
        {
            return signalBank_[track];
        }

        /**
         * @brief Returns the sum of all SignalBanks except for target track
         * Useful for calculating background
         *
         * @param target Track index
         *
         * @return SignalBank
         */
        SignalBank sumSignalBanksExcept(int target)

        /**
         * @brief Returns the state of the SignalBank.
         *
         * @return true if initialised, false otherwise.
         */
        bool isInitialized() const;

    private:

        int nTracks_, nChannels_, nSamples_, fs_;
        bool initialized_;
        RealVec signalBank_;
    }; 
}
#endif 

