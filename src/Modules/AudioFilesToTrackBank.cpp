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

#include "AudioFilesToTrackBank.h"

namespace loudness{

    AudioFilesToTrackBank::AudioFilesToTrackBank(vector<string> fileNames, int frameSize) :
        fileNames_(fileNames),
        frameSize_(frameSize),
        fs_(44100)
    {}
    
    AudioFilesToTrackBank::~AudioFilesToTrackBank()
    {}

    bool AudioFilesToTrackBank::initialize()
    {
        nTracks_ = fileNames_.size();
        for (int track = 0; track < nTracks_; track++)
        {
            files_.push_back(new AudioFileCutter(fileNames_[track], frameSize_));
            files_[track]->initialize();
        }

        nFrames_ = files_[0]->getNFrames();

        // CLEAN THIS
        output_.initialize(nTracks_, 1, frameSize_, fs_);
        return 1;
    }

    void AudioFilesToTrackBank::process()
    {

        for (int track=0; track < nTracks_; track++)
        {
            files_[track]->process();
            for (int smp = 0; smp < frameSize_; smp++)
                output_.setSample(track, 0, smp, files_[track]->getSample(0, smp));
        }
    }

    const TrackBank* AudioFilesToTrackBank::getOutput() const
    {
        return &output_;
    }

    int AudioFilesToTrackBank::getFs() const
    {
        return fs_;
    }

    int AudioFilesToTrackBank::getNFrames() const
    {
        return nFrames_;
    }
}
