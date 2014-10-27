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

#include "TrackBank.h"

namespace loudness{

    TrackBank::TrackBank()
    { 
        nTracks_ = 0;
        nChannels_ = 0;
        nSamples_ = 0;
        fs_ = 0;
        initialized_ = false;
        frameRate_ =0;
    }

    TrackBank::~TrackBank()
    {
    }

    void TrackBank::initialize(int nTracks, int nChannels, int nSamples, int fs)
    {
        nTracks_ = nTracks;
        nChannels_ = nChannels;
        nSamples_ = nSamples;
        fs_ = fs;
        frameRate_ = fs_/(Real)nSamples_;
        trig_.assign(nTracks_, 1);
        initialized_ = true;
        centreFreqs_.assign(nChannels_, 0.0);

        for (int track = 0; track < nTracks_; track++)
        {
            signal_[track].resize(nChannels_);
            for (int i=0; i<nChannels_; i++)
                signal_[track][i].assign(nSamples_,0.0);
        }
    }

    void TrackBank::initialize(const TrackBank &input)
    {
        if (input.isInitialized())
        {
            nTracks_ = input.getNTracks();
            nChannels_ = input.getNChannels();
            nSamples_ = input.getNSamples();
            fs_ = input.getFs();
            frameRate_ = input.getFrameRate();
            initialized_ = true;
            centreFreqs_ = input.getCentreFreqs();
            
            for (int track = 0; track < nTracks_; track++)
            {
                trig_[track] = input.getTrig(track);
                signal_[track].resize(nChannels_);
                for (int i=0; i<nChannels_; i++)
                    signal_[track][i].assign(nSamples_,0.0);
            }
        }
        else
        {
            LOUDNESS_ERROR("TrackBank: Input TrackBank not initialized!");
            initialized_ = false;
        }
    }

    void TrackBank::clear()
    {
        for(int track = 0; track < nTracks_; track++)
        {
            for(int i=0; i<nChannels_; i++)
                signal_[track][i].assign(nSamples_,0.0);
            trig_[track] = 1;
        }
    }

    void TrackBank::setFs(int fs)
    {
        fs_ = fs;
    }

    void TrackBank::setFrameRate(Real frameRate)
    {
        frameRate_ = frameRate;
    }

    void TrackBank::setCentreFreqs(const RealVec &centreFreqs)
    {
        centreFreqs_ = centreFreqs;
    }

    void TrackBank::setCentreFreq(int channel, Real freq)
    {
        if (channel<nChannels_)
           centreFreqs_[channel] = freq;
    }

    void TrackBank::setSignal(int track, int channel, const RealVec &signal)
    {
        if(channel<nChannels_ && (int)signal.size()==nSamples_ && track < nTracks_)
            signal_[track][channel] = signal;
        else
        {
            LOUDNESS_ERROR("TrackBank: "
                    << "Invalid channel index or signal lengths do not match, please correct.");
        }
    }

    const RealVec &TrackBank::getSignal(int track, int channel) const
    {
        return signal_[track][channel];
    }

    const RealVec &TrackBank::getCentreFreqs() const
    {
        return centreFreqs_;
    }

    int TrackBank::getFs() const
    {
        return fs_;
    }

    Real TrackBank::getFrameRate() const
    {
        return frameRate_;
    }

    bool TrackBank::isInitialized() const
    {
        return initialized_;
    }
}

