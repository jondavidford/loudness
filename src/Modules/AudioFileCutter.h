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

#ifndef AUDIOFILECUTTER_H
#define AUDIOFILECUTTER_H

#include "../Support/Module.h"
#include <sndfile.h>

#define MAXAUDIOBUFFERSIZE 192000

namespace loudness{

    /**
     * @class AudioFileCutter
     * 
     * @brief Extracts a frame of samples from an audio file and puts the result
     * into an output TrackBank for processing.
     *
     * This class uses an internal buffer to bring a block of samples into
     * memory, from which frames are extracted.
     *
     * Calling \ref process will generate a single frame of samples. If the
     * frame size is less than the length of the audio file or the end of the
     * file is reached, the frame is padded with zeros.
     *
     * @todo Mono checked out but need to validate for stereo files.
     * @todo Add MONOMIX case for downmixing stereo files.
     *
     * @author Dominic Ward
     */
    class AudioFileCutter : public Module
    {
    public:
        /**
         * @brief Constructs an AudioFileCutter object.
         *
         * @param fileName  Path to the audio file.
         * @param frameSize Number of samples in the frame.
         */
        AudioFileCutter(string fileName="", int frameSize = 512);

        virtual ~AudioFileCutter();
        virtual bool initialize();

        virtual void process();

        /**
         * @brief Returns the sampling frequency (Hz) of the loaded audio file.
         */
        int getFs() const;

        /**
         * @brief Returns the total number of samples comprising the entire audio file.
         */
        int getNSamples() const;

        /**
         * @brief Returns the duration of the audio file in seconds.
         */
        Real getDuration() const;

        /**
         * @brief Returns the total number of frames in the audio file.
         *
         * The final frame is padded with zeros if the length of the audio file
         * is not an integer multiple of the frame size.
         */
        int getNFrames() const;

    private:

        /* Hmmm */
        virtual bool initializeInternal(const TrackBank &input){return 0;};
        virtual void processInternal(const TrackBank &input){};
        /**
         * @brief Loads an audio file specified by @a fileName_;
         *
         * @param initialize Set to true if you want the output bank to be
         * reinitialised and false if not.
         */
        bool loadAudioFile(bool initialize);

        /**
         * @brief Calls the method loadAudioFile.
         */
        virtual void resetInternal();

        string fileName_;
        int frameSize_, nSamples_, nFrames_, fs_;
        int audioBufferSize_, bufferIdx_, frame_;
        SNDFILE* sndFile_;
        vector<float> audioBuffer_;
        Real duration_;

    };
}

#endif
