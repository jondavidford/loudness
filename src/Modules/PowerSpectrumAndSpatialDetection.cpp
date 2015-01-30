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

#include "PowerSpectrumAndSpatialDetection.h"
#include "../Support/AuditoryTools.h"

namespace loudness{

    PowerSpectrumAndSpatialDetection::PowerSpectrumAndSpatialDetection(
            const RealVec& bandFreqsHz, 
            const RealVec& windowSizeSecs, 
            bool uniform) :
        Module("PowerSpectrumAndSpatialDetection"),
        bandFreqsHz_(bandFreqsHz),
        windowSizeSecs_(windowSizeSecs),
        uniform_(uniform)
    {}

    PowerSpectrumAndSpatialDetection::~PowerSpectrumAndSpatialDetection()
    { 
        if(initialized_)
        {
            fftw_free(fftInputBufR_);
            fftw_free(fftOutputBufR_);
            fftw_free(fftInputBufL_);
            fftw_free(fftOutputBufL_);
            LOUDNESS_DEBUG(name_ << ": Buffers destroyed.");

            for(vector<fftw_plan>::iterator i = fftPlansR_.begin(); i != fftPlansR_.end(); i++)
                fftw_destroy_plan(*i);
            for(vector<fftw_plan>::iterator i = fftPlansL_.begin(); i != fftPlansL_.end(); i++)
                fftw_destroy_plan(*i);
            LOUDNESS_DEBUG(name_ << ": Plan(s) destroyed.");
        }
    }

    bool PowerSpectrumAndSpatialDetection::initializeInternal(const TrackBank &input)
    {
        if (input.getNTracks() % 2 == 0)
        {
            //number of windows
            nWindows_ = (int)windowSizeSecs_.size();

            //check input
            if(bandFreqsHz_.size() != (windowSizeSecs_.size()+1))
            {
                //Need to throw an exception, see Debug.h
                LOUDNESS_ERROR(name_ 
                        << ": Number of frequency bands should equal number of windows + 1.");
                return 0;
            }

            //sampling freqeuncy
            int fs = input.getFs();
            
            //window size in samples
            windowSizeSamps_.resize(nWindows_);
            int largestWindowSize = 0;
            for(int i=0; i<nWindows_; i++)
            {
                windowSizeSamps_[i] = round(fs*windowSizeSecs_[i]);
                if(windowSizeSamps_[i]>largestWindowSize)
                    largestWindowSize = windowSizeSamps_[i];
                LOUDNESS_DEBUG(name_ <<
                        ": Window size(s) in samples: " 
                        << windowSizeSamps_[i]);
            }

            LOUDNESS_DEBUG(name_ 
                    <<": Largest window size in samples: "
                    << largestWindowSize);

            //input size must be equal to the largest window
            if(input.getNSamples() != largestWindowSize)
            {
                LOUDNESS_ERROR(name_ << ": Number of input samples: " 
                        << input.getNSamples() 
                        << " but must be equal to: " 
                        << largestWindowSize);
                return 0;
            }

            //create N windows and associated fft plans
            windows_.resize(nWindows_);
            fftSize_.resize(nWindows_, 0);
        
            //allocate memory for FFT input buffers...all FFT inputs can make use of a single buffer
            //since we are not doing zero phase insersion
            fftSize_[0] = pow(2, ceil(log2(largestWindowSize)));
            fftInputBufR_ = (Real*) fftw_malloc(sizeof(Real) * fftSize_[0]);
            fftOutputBufR_ = (Real*) fftw_malloc(sizeof(Real) * fftSize_[0]);
            fftInputBufL_ = (Real*) fftw_malloc(sizeof(Real) * fftSize_[0]);
            fftOutputBufL_ = (Real*) fftw_malloc(sizeof(Real) * fftSize_[0]);
            
            //only 1 plan required if uniform spectral sampling 
            LOUDNESS_DEBUG(name_
                    << ": FFT buffer sizes: " 
                    << fftSize_[0] 
                    << ", memory allocated.");

            if(uniform_)
            {
                fftPlansR_.push_back(fftw_plan_r2r_1d(fftSize_[0],
                            fftInputBufR_, fftOutputBufR_, FFTW_R2HC, FFTW_PATIENT));
                fftPlansL_.push_back(fftw_plan_r2r_1d(fftSize_[0],
                            fftInputBufL_, fftOutputBufL_, FFTW_R2HC, FFTW_PATIENT));
                LOUDNESS_DEBUG(name_ <<
                        ": Created a single " 
                        << fftSize_[0] 
                        << "-point FFT plan for uniform spectral sampling.");
            }
            else
                LOUDNESS_DEBUG(name_ << ": Creating multiple plans...");

            //create windows and multiple plans (if needed)
            for(int i=0; i<nWindows_; i++)
            {
                if(!uniform_)
                {
                    fftSize_[i] = pow(2,ceil(log2(windowSizeSamps_[i])));
                    fftPlansR_.push_back(fftw_plan_r2r_1d(fftSize_[i],
                                fftInputBufR_, fftOutputBufR_, FFTW_R2HC, FFTW_PATIENT));
                    fftPlansL_.push_back(fftw_plan_r2r_1d(fftSize_[i],
                                fftInputBufL_, fftOutputBufL_, FFTW_R2HC, FFTW_PATIENT));
                }
                else
                    fftSize_[i] = fftSize_[0];

                //windows
                windows_[i].assign(windowSizeSamps_[i], 0.0);
                hannWindow(windows_[i], fftSize_[i]);
                LOUDNESS_DEBUG(name_ <<
                        ": Using window size " << windowSizeSamps_[i]
                        << ", for "  <<  fftSize_[i] << "-point FFT");
            }

            //desired bins indices (lo and hi) per band
            bandBinIndices_.resize(nWindows_);

            //appropriate delay for temporal alignment
            temporalCentre_ = (largestWindowSize-1)/2.0;
            LOUDNESS_DEBUG(name_ 
                    <<": Temporal centre of largest window: " 
                    << temporalCentre_);

            //the rest
            windowDelay_.resize(nWindows_);
            for(int i=0; i<nWindows_; i++)
            {
                //TrackBank offset for temporal alignment
                Real tc2 = (windowSizeSamps_[i]-1)/2.0;
                windowDelay_[i] = (int)round(temporalCentre_ - tc2);

                LOUDNESS_DEBUG(name_ 
                        << ": Window of size " << windowSizeSamps_[i] 
                        << " samples requires a " << windowDelay_[i] 
                        << " sample delay."
                        << " Centre point (samples): " 
                        << tc2+windowDelay_[i]);
                
                //bin indices to use for compiled spectrum
                bandBinIndices_[i].resize(2);
                //These are NOT the nearest components but satisfies f_k in [f_lo, f_hi)
                bandBinIndices_[i][0] = ceil(bandFreqsHz_[i]*fftSize_[i]/fs);
                bandBinIndices_[i][1] = ceil(bandFreqsHz_[i+1]*fftSize_[i]/fs)-1;

                if(bandBinIndices_[i][1]==0)
                {
                    LOUDNESS_ERROR(name_ << ": No components found in band number " << i);
                    return 0;
                }

                //exclude DC and Nyquist if found
                if(bandBinIndices_[i][0]==0)
                {
                    LOUDNESS_WARNING(name_ << ": DC found...excluding.");
                    bandBinIndices_[i][0] = 1;
                }
                if(bandBinIndices_[i][1] >= (fftSize_[i]/2.0))
                {
                    LOUDNESS_WARNING(name_ << 
                            ": Bin is >= nyquist...excluding.");
                    bandBinIndices_[i][1] = (ceil(fftSize_[i]/2.0)-1);
                }
            }

            //count total number of bins and ensure no overlap
            int nBins = 0;
            for(int i=1; i<nWindows_; i++)
            {
                while((bandBinIndices_[i][0]*fs/fftSize_[i]) <= (bandBinIndices_[i-1][1]*fs/fftSize_[i-1]))
                    bandBinIndices_[i][0] += 1;

                //this line will alter the band frequencies slightly to ensure closely spaced bins
                /*
                 while(((bandBinIndices_[i-1][1]+1)*fs/fftSize_[i-1]) < (bandBinIndices_[i][0]*fs/fftSize_[i]))
                    bandBinIndices_[i-1][1] += 1;   
                */
                nBins += bandBinIndices_[i-1][1]-bandBinIndices_[i-1][0] + 1;
            }
            
            //total number of bins in the output spectrum
            nBins += bandBinIndices_[nWindows_-1][1]-bandBinIndices_[nWindows_-1][0] + 1;

            LOUDNESS_DEBUG(name_ 
                    << ": Total number of bins comprising the output spectrum: " << nBins);

            #if defined(DEBUG)
            for(int i=0; i<nWindows_; i++)
            {
                Real edgeLo = bandBinIndices_[i][0]*fs/(Real)fftSize_[i];
                Real edgehi = bandBinIndices_[i][1]*fs/(Real)fftSize_[i];
                LOUDNESS_DEBUG(name_ 
                        << ": Band interval (Hz) for window of size: " 
                        << windowSizeSamps_[i] << " = [ "
                        << std::setprecision (7) 
                        << edgeLo << ", " 
                        <<  edgehi 
                        << " ].");
            }
            #endif 

            // initialize the output TrackBank
            // we are combining L and R channels in to mono (nTracks / 2)
            // AND adding an additional "background maskers" track
            // consisting of every other track (each amplitude reduced
            // according to the function from fig 3 of Kidd 2002).
            // this is another (nTracks / 2),
            // so we end up with nTracks
            nTracks_ = input.getNTracks();
            nInputs_ = nTracks_ / 2;
            nBins_ = nBins;

            output_.initialize(nTracks_, nBins_, 1, fs);
            output_.setFrameRate(input.getFrameRate());

            // also initialize real and imaginary buffers
            // we only need one for each pair of tracks
            maskerReal_.resize(nBins_);
            maskerImag_.resize(nBins_);
            avgPositions_.resize(nInputs_);
            real_.resize(nInputs_);
            imag_.resize(nInputs_);
            for (int i = 0; i < nInputs_; i++)
            {
                real_[i].resize(nBins_);
                imag_[i].resize(nBins_);
            }

            //output frequencies in Hz
            int j = 0, k = 0;
            for(int i=0; i<nWindows_; i++)
            {
                j = bandBinIndices_[i][0];
                while(j <= bandBinIndices_[i][1])
                    output_.setCentreFreq(k++, (j++)*fs/(Real)fftSize_[i]);
            }

            return 1;
        }
        else
        {
            LOUDNESS_ERROR(name_
                << ": TrackBank must have an even number of tracks")
            return 0;
        }
    }

    void PowerSpectrumAndSpatialDetection::processInternal(const TrackBank &input)
    {
        // first perform ffts on every track
        for (int track=0; track < nInputs_; track++)
        {
            //for each window
            int binWriteIdx = 0;
            const int lIdx = track * 2;
            const int rIdx = track * 2 + 1;
            Real positionSum = 0;
            for(int i=0; i<nWindows_; i++)
            {
                //fill the buffers
                for(int j=0; j<windowSizeSamps_[i]; j++)
                {
                    fftInputBufL_[j] = input.getSample(lIdx, 0, windowDelay_[i]+j)*windows_[i][j];
                    fftInputBufR_[j] = input.getSample(rIdx, 0, windowDelay_[i]+j)*windows_[i][j];
                }

                //compute ffts
                if(uniform_)
                {
                    fftw_execute(fftPlansL_[0]);
                    fftw_execute(fftPlansR_[0]);
                }
                else
                {
                    fftw_execute(fftPlansL_[i]);
                    fftw_execute(fftPlansR_[i]);
                }

                //clear windowed data
                for(int j=0; j<windowSizeSamps_[i]; j++)
                {
                    fftInputBufL_[j] = 0.0;
                    fftInputBufR_[j] = 0.0;
                }

                //Extract components from band and compute powers
                Real reR, reL, imR, imL, magR, magL, pos;
                for(int j=bandBinIndices_[i][0]; j<=bandBinIndices_[i][1]; j++)
                {
                    // left channel
                    reL = fftOutputBufL_[j];
                    imL = fftOutputBufL_[fftSize_[i]-j];
                    magL = sqrt(pow(reL,2) + pow(imL, 2));

                    // right channel
                    reR = fftOutputBufR_[j];
                    imR = fftOutputBufR_[fftSize_[i]-j];
                    magR = sqrt(pow(reR,2) + pow(imR, 2));

                    // calculate spatial position
                    // 0 is all the way left
                    // 180 is all the way right
                    if (magL == 0) // signal all the way left
                    {
                        pos = 0;
                    }
                    else if  (magR == 0) // signal all the way right
                    {
                        pos = 180.0;
                    }
                    else if (magL > magR) // signal partially left
                    {
                        pos = (magR / magL) * 90.0;
                    }
                    else // signal partially right
                    {
                        pos = (1.0 - (magL / magR)) * 90.0 + 90.0;
                    }
                    output_.setSpatialPosition(track, binWriteIdx, pos);
                    positionSum += pos;

                    // set outputs
                    real_[track][binWriteIdx] = reL + reR;
                    imag_[track][binWriteIdx] = imL + imR;
                    output_.setSample(track, binWriteIdx, 0, pow((magL + magR) / 2, 2)); 
                    binWriteIdx++;
                }
            }

            // calculate average position of track
            avgPositions_[track] = positionSum / binWriteIdx;
        }

        // next sum tracks to create background masker tracks
        Real separation, reduction;
        for (int target = 0; target < nInputs_; target++)
        {
            // initialize masker track with 0's in real and imag components
            const int maskerWriteIdx = target + nInputs_;
            maskerReal_.assign(nBins_, 0.0);
            maskerImag_.assign(nBins_, 0.0);
            
            // reduce and sum the components for each bin
            for (int masker = 0; masker < nInputs_; masker++)
            {
                if (target != masker)
                {
                    separation = abs(avgPositions_[target] - avgPositions_[masker]);
                    reduction = separationToReduction(separation);
                    for (int bin = 0; bin < nBins_; bin++)
                    {
                        maskerReal_[bin] += real_[masker][bin] * reduction;
                        maskerImag_[bin] += imag_[masker][bin] * reduction;
                    }
                }
            }

            // set output
            for (int bin = 0; bin < nBins_; bin++)
                output_.setSample(maskerWriteIdx, bin, 0, pow(maskerReal_[bin], 2) + pow(maskerImag_[bin], 2));
        }
    }

    void PowerSpectrumAndSpatialDetection::hannWindow(RealVec &w, int fftSize)
    {
        int windowSize = (int)w.size();
        Real norm = sqrt(2.0/(fftSize*windowSize*0.375*2e-5*2e-5));

        for(int i=0; i< windowSize; i++)
            w[i] = norm*(0.5+0.5*cos(2*PI*(i-0.5*(windowSize-1))/windowSize));
    }

    void PowerSpectrumAndSpatialDetection::resetInternal()
    {
        for(int i=0; i<nWindows_; i++)
            windowDelay_[i] = (int)round(temporalCentre_ - (windowSizeSamps_[i]-1)/2.0);
    }
}

