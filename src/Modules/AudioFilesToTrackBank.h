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

#ifndef AUDIOFILESTOTRACKBANK_H
#define AUDIOFILESTOTRACKBANK_H

#include "../Support/TrackBank.h"
#include "AudioFileCutter.h"

namespace loudness{

    /**
     * @class AudioFilesToTrackBank
     * 
     * @brief Creates a vector of AudioFilesToTrackBank objects. The process
     * function will place output from objects into a TrackBank
     *
     *
     * @author Dominic Ward
     */
    class AudioFilesToTrackBank
    {
    public:
        /**
         * @brief Constructs an AudioFilesToTrackBank object.
         *
         * @param fileName  Path to the audio file.
         * @param frameSize Number of samples in the frame.
         */
        AudioFilesToTrackBank(vector<string> fileNames, int frameSize = 512);

        ~AudioFilesToTrackBank();
        bool initialize();

        void process();

        const TrackBank* getOutput() const;

        /**
         * @brief Returns the sampling frequency (Hz) of the loaded audio file.
         */
        int getFs() const;

        int getNFrames() const;

    private:

        TrackBank output_;
        vector<string> fileNames_;
        vector<AudioFileCutter*> files_;
        int frameSize_, fs_, nTracks_, nFrames_;
    };
}

#endif
