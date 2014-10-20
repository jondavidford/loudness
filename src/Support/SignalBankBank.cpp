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

#include "SignalBankBank.h"

namespace loudness{

    SignalBankBank::SignalBankBank()
    { 
        nChannels_ = 0;
        nSamples_ = 0;
        nTracks_ = 0;
        fs_ = 0;
        trig_ = 0;
        initialized_ = false;
        frameRate_ =0;
    }

    SignalBankBank::~SignalBankBank()
    {
    }

    void SignalBankBank::initialize(int nTracks, int nChannels, int nSamples, int fs)
    {
        nChannels_ = nChannels;
        nSamples_ = nSamples;
        nTracks_ = nTracks;
        fs_ = fs;
        initialized_ = true;

        emptySignalBank = SignalBank(nChannels, nSamples, fs)
        signalBank_.resize(nTracks_);
        signalBank_.assign(nTracks_, emptySignalBank);
    }

    void SignalBankBank::initialize(const SignalBankBank &input)
    {
        if(input.isInitialized())
        {
            nChannels_ = input.getNChannels();
            nSamples_ = input.getNSamples();
            fs_ = input.getFs();
            initialized_ = true;
            emptySignalBank = SignalBank(nChannels, nSamples, fs)
            signalBank_.resize(nTracks_);
            signalBank_.assign(nTracks_, emptySignalBank);
        }
        else
        {
            LOUDNESS_ERROR("SignalBankBank: Input SignalBankBank not initialized!");
            initialized_ = false;
        }
    }

    void SignalBankBank::setFs(int fs)
    {
        fs_ = fs;
    }

    int SignalBankBank::getFs() const
    {
        return fs_;
    }

    bool SignalBankBank::isInitialized() const
    {
        return initialized_;
    }

    SignalBank sumSignalBanksExcept(int target)
    {
        // initialize empty signal bank
        masker = SignalBank(nChannels_, nSamples_, fs_)

        // initialize 
        for(track=0; track<nTracks_; track++)
        {
            if(track != target)
            {

            }
        }
    }
}

