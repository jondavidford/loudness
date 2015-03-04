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

#include "FrameGenerator.h"

namespace loudness{

    FrameGenerator::FrameGenerator(int frameSize, int hopSize) :
        Module("FrameGenerator"),
        frameSize_(frameSize),
        hopSize_(hopSize)
    {}
    
    FrameGenerator::~FrameGenerator()
    {
    }

    bool FrameGenerator::initializeInternal(const TrackBank &input)
    {
        LOUDNESS_WARNING(name_ 
                        << ": nTracks: " << input.getNTracks());
        if(hopSize_ > frameSize_)
        {
            LOUDNESS_ERROR(name_ 
                    << ": Hop size cannot be greater than frame size");
            return 0;
        }

        //number of input samples per process call
        inputBufferSize_ = input.getNSamples();
        
        //Hop size checks
        LOUDNESS_DEBUG(name_ << ": Input buffer size in samples: " << inputBufferSize_);
        if(inputBufferSize_ > hopSize_)
        {
            LOUDNESS_WARNING(name_ << 
                    ": Hop size cannot be greater than input buffer size" 
                    << "...automatically correcting.");
        }
        else if((hopSize_ % inputBufferSize_)>0)
        {
            LOUDNESS_WARNING(name_ << 
                    ": Hop size is not an integer multiple of the input buffer size" 
                    << "...automatically correcting.");
        }
        
        hopSize_ = inputBufferSize_*ceil(hopSize_/(Real) inputBufferSize_);
        LOUDNESS_DEBUG(name_ << ": Hop size in samples: " << hopSize_);

        LOUDNESS_DEBUG(name_ << ": Frame size in samples: " << frameSize_);
    
        //The audio buffer must also be an integer multiple of inputBufSize_
        audioBufferSize_ = inputBufferSize_* ceil(frameSize_/(Real)inputBufferSize_);

        //OK, allocate memory
        nTracks_ = input.getNTracks();
        audioBuffer_.resize(nTracks_);
        for (int track = 0; track < nTracks_; track++)
            audioBuffer_[track].assign(audioBufferSize_, 0.0);

        LOUDNESS_DEBUG(name_ << 
                ": Audio buffer size in samples: " 
                << audioBufferSize_);
        
        //Number of frames until we reach the end of buffer
        initNFramesFull_.assign(nTracks_, audioBufferSize_ / inputBufferSize_);
        nFramesFull_.assign(nTracks_, hopSize_ / inputBufferSize_); //will be an int >= 1
        LOUDNESS_DEBUG(name_
                << ": Number of process calls until we can extract first frame: " 
                << initNFramesFull_
                << "\n Number of process calls until we can extract further frames: "
                << nFramesFull_);

        readIdx_.assign(nTracks_, 0);
        writeIdx_.assign(nTracks_, 0);
        count_.assign(nTracks_, 0);

        //initialise the output signal
        output_.initialize(nTracks_, 1, frameSize_, input.getFs());
        output_.setFrameRate(input.getFs()/(Real)hopSize_);

        return 1;
    }

    void FrameGenerator::processInternal(const TrackBank &input)
    {

        //fill internal buffer
        for (int track=0; track < nTracks_; track++)
        {
            for(int i=0; i<inputBufferSize_; i++)
                audioBuffer_[track][writeIdx_[track]++] = input.getSample(track, 0, i);
            //wrap write index
            writeIdx_[track] = writeIdx_[track] % audioBufferSize_;

            count_[track]++;
            if(count_[track] == initNFramesFull_[track])
            {
                LOUDNESS_DEBUG(name_ << ": readIdx_ : " << readIdx_[track]);

                //output frame
                for(int i=0; i<frameSize_; i++)
                {
                    output_.setSample(track, 0, i, audioBuffer_[track][readIdx_[track]++]);
                    readIdx_[track] = readIdx_[track] % audioBufferSize_;
                }
                readIdx_[track] = (writeIdx_[track] + hopSize_) % audioBufferSize_;

                output_.setTrig(track, 1);

                count_[track] = 0;
                initNFramesFull_[track] = nFramesFull_[track];
            }
            else
                output_.setTrig(track, 0);
        }
    }

    void FrameGenerator::resetInternal()
    {
        initNFramesFull_.assign(nTracks_, audioBufferSize_ / inputBufferSize_);
        nFramesFull_.assign(nTracks_, hopSize_ / inputBufferSize_);
        readIdx_.assign(nTracks_, 0);
        writeIdx_.assign(nTracks_, 0);
        count_.assign(nTracks_, 0);
    }

    void FrameGenerator::setFrameSize(int frameSize)
    {
        frameSize_ = frameSize;
    }

    void FrameGenerator::setHopSize(int hopSize)
    {
        hopSize_ = hopSize;
    }

    int FrameGenerator::getFrameSize() const
    {
        return frameSize_;
    }

    int FrameGenerator::getHopSize() const
    {
        return hopSize_;
    }

    int FrameGenerator::getAudioBufferSize() const
    {
        return audioBufferSize_;
    }
}
