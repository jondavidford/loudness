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

#include "AudioFileCutter.h"

namespace loudness{

    AudioFileCutter::AudioFileCutter(vector<string> fileNames, int frameSize) :
        Module("AudioFileCutter"),
        fileNames_(fileNames),
        frameSize_(frameSize),
        fs_(0)
    {}
    
    AudioFileCutter::~AudioFileCutter()
    {
        if(initialized_)
        {
            //delete [] audioBuffer_;
            for (int i = 0; i < nFiles_; i++)
            {
                if(sf_close(sndFiles_[i])>0)
                    LOUDNESS_ERROR("AudioFileCutter: Error closing audio file");
            }
        }
    }

    bool AudioFileCutter::initialize()
    {
        nFiles_ = fileNames_.size();
        sndFiles_.resize(nFiles_);
        if(loadAudioFiles(true))
        {
            LOUDNESS_DEBUG("Module: Module initialized.");
            if(output_.isInitialized())
            {
                if(targetModule_)
                    targetModule_ -> initialize(output_);
            }
            initialized_ = 1;
            return 1;
        }
        else
        {
            LOUDNESS_ERROR("Module: Module not initialized!");
            return 0;
        }
    }

    bool AudioFileCutter::loadAudioFiles(bool initialize)
    {
        for (int i = 0; i < nFiles_; i++)
        {
            // If the file is already opened, first close it
            if (sf_close(sndFiles_[i])>0)
            {
                LOUDNESS_ERROR("AudioFileCutter: Error closing Audio file.");
                return 0;
            }

            // Check we have a filename
            if(fileNames_[i].empty())
            {
                LOUDNESS_ERROR("AudioFileCutter: No file name!");
                return 0;
            }
            else
            {
                // Open the audio file
                SF_INFO fileInfo;
                fileInfo.format = 0;
                sndFiles_[i] = sf_open(fileNames_[i].c_str(), SFM_READ, &fileInfo);

                if (!sndFiles_[i]) //if null
                {
                    LOUDNESS_ERROR("AudioFileCutter: Failed to open audio file: " 
                            << fileNames_[i] << ", " 
                            << sf_strerror(sndFiles_[i]));
                    return 0;
                }
                else
                {
                    LOUDNESS_DEBUG("AudioFileCutter: Audio file successfully loaded.");

                    //total number of samples in the audio file and number of frames
                    nSamples_ = fileInfo.frames;
                    nFrames_ = ceil(nSamples_ / (float)frameSize_);
                    duration_ = nSamples_/(Real)fileInfo.samplerate;
                    fs_ = fileInfo.samplerate;

                    //force audio buffer size to be a multiple of frameSize_
                    if(nSamples_ < MAXAUDIOBUFFERSIZE)
                    {
                        audioBufferSize_ = ceil(nSamples_ / (float)frameSize_) * frameSize_;
                    }
                    else
                    {
                        audioBufferSize_ = MAXAUDIOBUFFERSIZE * fileInfo.channels;
                        audioBufferSize_ = ceil(audioBufferSize_ / (float)frameSize_) * frameSize_;
                    }
                    LOUDNESS_DEBUG("AudioFileCutter: Audio buffer size: " << audioBufferSize_);
                    bufferIdx_ = 0;
                    frame_ = 0;

                    if(initialize)
                    {
                        output_.initialize(nFiles_, fileInfo.channels, frameSize_,
                                fileInfo.samplerate);
                        audioBuffer_.resize(audioBufferSize_);
                        initialize = false;
                    }
                    else
                    {
                        if(fileInfo.samplerate!=output_.getFs())
                        {
                            LOUDNESS_WARNING("AudioFileCutter: Sampling frequency: " 
                                    << fileInfo.samplerate << " but previous was: "
                                    << output_.getFs() << ". Continuing anyway...");
                        }
                        if(fileInfo.channels!=output_.getNChannels())
                        {
                            LOUDNESS_WARNING("AudioFileCutter: Number of channels: " 
                                    << fileInfo.channels << " but previous was: "
                                    << output_.getFs() << ". Continuing anyway...");
                        }
                    }
                }
            }
        }
        return 1;
    }

    void AudioFileCutter::process()
    {
        //clear the output signal bank
        output_.clear();

        int nSamples = output_.getNSamples();
        int nChannels = output_.getNChannels();

        //if initialised and we have an audio file
        for (int file = 0; file < nFiles_; file++)
        {
            if(initialized_ && sndFiles_[file])
            {
                //If we have extracted all data from buffer, get more
                if((bufferIdx_ == 0) || (bufferIdx_ == audioBufferSize_))
                {
                    int readCount = sf_readf_float(sndFiles_[file], &audioBuffer_[0], audioBufferSize_);

                    //Zero pad when towards the end
                    if(readCount<audioBufferSize_)
                        fill(audioBuffer_.begin() + readCount, audioBuffer_.end(),0.0);

                    LOUDNESS_DEBUG("AudioFileCutter: Number of samples extracted: " << readCount);

                    frame_ = 0;
                    bufferIdx_ = 0;
                }  

                LOUDNESS_DEBUG("AudioFileCutter: frame: " << frame_ 
                        << " buffer index: " << bufferIdx_);

                //Fill the output signal bank
                for(int i=0; i<nSamples; i += nChannels)
                {
                    for(int j=0; j<nChannels; j++)
                        output_.setSample(file, j, i, audioBuffer_[bufferIdx_ + i + j]);
                }         
            }
        }
        //increment the frame counter
        frame_ += 1;

        //update buffer index
        bufferIdx_ = frame_ * nSamples * nChannels;

        //push through processing pipeline if necessary
        if(targetModule_)
            targetModule_->process(output_);
    }

    void AudioFileCutter::resetInternal()
    {
        loadAudioFiles(false);
    }

    int AudioFileCutter::getNSamples() const
    {
        return nSamples_;
    }

    Real AudioFileCutter::getDuration() const
    {
        return duration_;
    }

    int AudioFileCutter::getFs() const
    {
        return fs_;
    }

    int AudioFileCutter::getNFrames() const
    {
        return nFrames_;
    }


}
